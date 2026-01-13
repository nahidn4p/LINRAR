#include "FileBrowser.h"
#include "utils/ArchiveUtils.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QMenu>
#include <QFileInfo>
#include <QMessageBox>

FileBrowser::FileBrowser(QWidget *parent)
    : QWidget(parent), contextMenu(nullptr) {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Toolbar
    toolbar = new QToolBar(this);
    QAction *upAction = toolbar->addAction(tr("Up"));
    upAction->setToolTip(tr("Go up one directory"));
    connect(upAction, &QAction::triggered, this, &FileBrowser::goUp);
    
    QAction *homeAction = toolbar->addAction(tr("Home"));
    homeAction->setToolTip(tr("Go to home directory"));
    connect(homeAction, &QAction::triggered, this, &FileBrowser::goHome);
    
    toolbar->addSeparator();
    layout->addWidget(toolbar);
    
    // Path edit
    pathEdit = new QLineEdit(this);
    pathEdit->setPlaceholderText(tr("Path"));
    pathEdit->setToolTip(tr("Enter path and press Enter to navigate"));
    connect(pathEdit, &QLineEdit::returnPressed, this, &FileBrowser::onPathChanged);
    layout->addWidget(pathEdit);
    
    // Tree view
    treeView = new QTreeView(this);
    model = new QFileSystemModel(this);
    model->setRootPath(QDir::homePath());
    model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    
    treeView->setModel(model);
    treeView->setRootIndex(model->index(QDir::homePath()));
    treeView->setHeaderHidden(false);
    treeView->setAnimated(true);
    treeView->setIndentation(20);
    treeView->setSortingEnabled(true);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    treeView->sortByColumn(0, Qt::AscendingOrder);
    
    connect(treeView, &QTreeView::doubleClicked, this, &FileBrowser::onItemDoubleClicked);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FileBrowser::onSelectionChanged);
    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &FileBrowser::showContextMenu);
    
    setupContextMenu();
    layout->addWidget(treeView);
    
    setCurrentPath(QDir::homePath());
}

QString FileBrowser::getCurrentPath() const {
    QModelIndex index = treeView->currentIndex();
    if (index.isValid()) {
        return model->filePath(index);
    }
    return model->rootPath();
}

void FileBrowser::setCurrentPath(const QString &path) {
    QModelIndex index = model->index(path);
    if (index.isValid()) {
        treeView->setCurrentIndex(index);
        treeView->setRootIndex(model->parent(index));
        pathEdit->setText(path);
    }
}

QStringList FileBrowser::getSelectedFiles() const {
    QStringList files;
    QModelIndexList indexes = treeView->selectionModel()->selectedIndexes();
    
    for (const QModelIndex &index : indexes) {
        if (index.column() == 0) { // Only process once per row
            QString path = model->filePath(index);
            if (!files.contains(path)) {
                files.append(path);
            }
        }
    }
    
    return files;
}

void FileBrowser::setShowHiddenFiles(bool show) {
    if (show) {
        model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    } else {
        model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    }
}

void FileBrowser::onItemDoubleClicked(const QModelIndex &index) {
    QString path = model->filePath(index);
    if (model->isDir(index)) {
        treeView->setRootIndex(index);
        pathEdit->setText(path);
        emit directoryChanged(path);
    } else {
        emit fileSelected(path);
    }
}

void FileBrowser::onSelectionChanged() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit filesSelected(files);
    }
}

void FileBrowser::onPathChanged() {
    QString path = pathEdit->text();
    QDir dir(path);
    if (dir.exists()) {
        setCurrentPath(path);
    }
}

void FileBrowser::goUp() {
    QModelIndex currentRoot = treeView->rootIndex();
    if (currentRoot.isValid()) {
        QModelIndex parent = model->parent(currentRoot);
        if (parent.isValid()) {
            treeView->setRootIndex(parent);
            pathEdit->setText(model->filePath(parent));
            emit directoryChanged(model->filePath(parent));
        }
    }
}

void FileBrowser::goHome() {
    QString homePath = QDir::homePath();
    setCurrentPath(homePath);
    treeView->setRootIndex(model->index(homePath));
    emit directoryChanged(homePath);
}

void FileBrowser::setupContextMenu() {
    contextMenu = new QMenu(this);
    
    QAction *addAction = contextMenu->addAction(tr("Add to Archive"), this, &FileBrowser::onAddToArchive);
    addAction->setToolTip(tr("Add selected files to the current archive"));
    
    contextMenu->addSeparator();
    
    QAction *propertiesAction = contextMenu->addAction(tr("Properties"), this, &FileBrowser::onProperties);
    propertiesAction->setToolTip(tr("Show file properties"));
}

void FileBrowser::showContextMenu(const QPoint &pos) {
    QModelIndex index = treeView->indexAt(pos);
    if (index.isValid()) {
        // Select the item if not already selected
        if (!treeView->selectionModel()->isSelected(index)) {
            treeView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
        contextMenu->exec(treeView->mapToGlobal(pos));
    }
}

void FileBrowser::onAddToArchive() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit addToArchiveRequested(files);
    }
}

void FileBrowser::onProperties() {
    QModelIndexList indexes = treeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }
    
    QModelIndex index = indexes.first();
    if (index.column() != 0) {
        index = model->index(index.row(), 0, index.parent());
    }
    
    QString path = model->filePath(index);
    QFileInfo info(path);
    
    QString fileType = info.isDir() ? tr("Directory") : tr("File");
    QString size = info.isDir() ? tr("N/A") : ArchiveUtils::formatFileSizeString(info.size());
    
    QString permissions;
    permissions += (info.permissions().testFlag(QFile::ReadOwner) ? "r" : "-");
    permissions += (info.permissions().testFlag(QFile::WriteOwner) ? "w" : "-");
    permissions += (info.permissions().testFlag(QFile::ExeOwner) ? "x" : "-");
    
    QString infoText = tr("File Properties\n\n"
                         "Name: %1\n"
                         "Path: %2\n"
                         "Size: %3\n"
                         "Type: %4\n"
                         "Modified: %5\n"
                         "Permissions: %6")
                      .arg(info.fileName())
                      .arg(path)
                      .arg(size)
                      .arg(fileType)
                      .arg(info.lastModified().toString())
                      .arg(permissions);
    
    QMessageBox::information(this, tr("Properties"), infoText);
    emit propertiesRequested(path);
}
