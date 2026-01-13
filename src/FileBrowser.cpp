#include "FileBrowser.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QAction>

FileBrowser::FileBrowser(QWidget *parent)
    : QWidget(parent) {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Toolbar
    toolbar = new QToolBar(this);
    QAction *upAction = toolbar->addAction(tr("Up"));
    connect(upAction, &QAction::triggered, this, &FileBrowser::goUp);
    
    QAction *homeAction = toolbar->addAction(tr("Home"));
    connect(homeAction, &QAction::triggered, this, &FileBrowser::goHome);
    
    toolbar->addSeparator();
    layout->addWidget(toolbar);
    
    // Path edit
    pathEdit = new QLineEdit(this);
    pathEdit->setPlaceholderText(tr("Path"));
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
    treeView->sortByColumn(0, Qt::AscendingOrder);
    
    connect(treeView, &QTreeView::doubleClicked, this, &FileBrowser::onItemDoubleClicked);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &FileBrowser::onSelectionChanged);
    
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
