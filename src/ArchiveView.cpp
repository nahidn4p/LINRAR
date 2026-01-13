#include "ArchiveView.h"
#include "utils/ArchiveUtils.h"
#include <QHeaderView>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QTemporaryDir>
#include <QProcess>

ArchiveView::ArchiveView(QWidget *parent)
    : QWidget(parent), currentHandler(nullptr), contextMenu(nullptr) {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    archiveLabel = new QLabel(tr("No archive open"), this);
    archiveLabel->setStyleSheet("font-weight: bold; padding: 5px;");
    layout->addWidget(archiveLabel);
    
    treeView = new QTreeView(this);
    model = new ArchiveModel(this);
    treeView->setModel(model);
    treeView->setHeaderHidden(false);
    treeView->setAnimated(true);
    treeView->setIndentation(20);
    treeView->setRootIsDecorated(true);
    treeView->setAlternatingRowColors(true);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    treeView->header()->setStretchLastSection(false);
    treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    
    connect(treeView, &QTreeView::doubleClicked, this, &ArchiveView::onItemDoubleClicked);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ArchiveView::onSelectionChanged);
    connect(treeView, &QTreeView::customContextMenuRequested,
            this, &ArchiveView::showContextMenu);
    
    setupContextMenu();
    layout->addWidget(treeView);
}

void ArchiveView::setArchive(const QString &archivePath, ArchiveHandler *handler) {
    if (!handler || !handler->isAvailable()) {
        QMessageBox::warning(this, tr("Error"), 
                            tr("Archive handler not available for this format."));
        return;
    }
    
    currentArchivePath = archivePath;
    currentHandler = handler;
    
    QFileInfo info(archivePath);
    archiveLabel->setText(tr("Archive: %1").arg(info.fileName()));
    
    QList<ArchiveEntry> entries;
    if (handler->list(archivePath, entries)) {
        model->setEntries(entries);
        treeView->expandAll();
    } else {
        QMessageBox::warning(this, tr("Error"), 
                            tr("Failed to read archive."));
        clear();
    }
    
    emit archiveChanged(archivePath);
}

void ArchiveView::clear() {
    currentArchivePath.clear();
    currentHandler = nullptr;
    archiveLabel->setText(tr("No archive open"));
    model->clear();
}

QStringList ArchiveView::getSelectedFiles() const {
    QStringList files;
    QModelIndexList indexes = treeView->selectionModel()->selectedIndexes();
    
    for (const QModelIndex &index : indexes) {
        if (index.column() == 0) { // Only process once per row
            ArchiveEntry entry = model->getEntry(index);
            if (!entry.path.isEmpty() && !entry.isDirectory) {
                files.append(entry.path);
            }
        }
    }
    
    return files;
}

void ArchiveView::onItemDoubleClicked(const QModelIndex &index) {
    ArchiveEntry entry = model->getEntry(index);
    if (!entry.path.isEmpty()) {
        emit fileSelected(entry.path);
    }
}

void ArchiveView::onSelectionChanged() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit filesSelected(files);
    }
}

void ArchiveView::setupContextMenu() {
    contextMenu = new QMenu(this);
    
    QAction *extractAction = contextMenu->addAction(tr("Extract"), this, &ArchiveView::onExtract);
    extractAction->setToolTip(tr("Extract selected files"));
    
    QAction *extractToAction = contextMenu->addAction(tr("Extract To..."), this, &ArchiveView::onExtractTo);
    extractToAction->setToolTip(tr("Extract selected files to a specific location"));
    
    contextMenu->addSeparator();
    
    QAction *deleteAction = contextMenu->addAction(tr("Delete"), this, &ArchiveView::onDelete);
    deleteAction->setToolTip(tr("Remove selected files from archive"));
    
    contextMenu->addSeparator();
    
    QAction *openWithAction = contextMenu->addAction(tr("Open With..."), this, &ArchiveView::onOpenWith);
    openWithAction->setToolTip(tr("Open file with external application"));
    
    QAction *propertiesAction = contextMenu->addAction(tr("Properties"), this, &ArchiveView::onProperties);
    propertiesAction->setToolTip(tr("Show file properties"));
}

void ArchiveView::showContextMenu(const QPoint &pos) {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    QModelIndex index = treeView->indexAt(pos);
    if (index.isValid()) {
        // Select the item if not already selected
        if (!treeView->selectionModel()->isSelected(index)) {
            treeView->selectionModel()->select(index, QItemSelectionModel::Select);
        }
        contextMenu->exec(treeView->mapToGlobal(pos));
    }
}

void ArchiveView::onExtract() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit extractRequested(files);
    }
}

void ArchiveView::onExtractTo() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit extractToRequested(files);
    }
}

void ArchiveView::onDelete() {
    QStringList files = getSelectedFiles();
    if (!files.isEmpty()) {
        emit deleteRequested(files);
    }
}

void ArchiveView::onOpenWith() {
    QModelIndexList indexes = treeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }
    
    // Get the first selected file
    QModelIndex index = indexes.first();
    if (index.column() != 0) {
        // Get the name column
        index = model->index(index.row(), 0, index.parent());
    }
    
    ArchiveEntry entry = model->getEntry(index);
    if (!entry.path.isEmpty() && !entry.isDirectory && currentHandler) {
        // Extract to temp and open
        QTemporaryDir tempDir;
        if (tempDir.isValid()) {
            QString tempPath = tempDir.path();
            QStringList files;
            files << entry.path;
            if (currentHandler->extract(currentArchivePath, tempPath, files)) {
                // Handle path - entry.path might have subdirectories
                QString extractedFile = tempPath + "/" + entry.path;
                QFileInfo fileInfo(extractedFile);
                if (fileInfo.exists()) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
                } else {
                    // Try just the filename if path doesn't exist
                    QString fileName = QFileInfo(entry.path).fileName();
                    QString altPath = tempPath + "/" + fileName;
                    if (QFileInfo::exists(altPath)) {
                        QDesktopServices::openUrl(QUrl::fromLocalFile(altPath));
                    }
                }
            }
        }
    }
}

void ArchiveView::onProperties() {
    QModelIndexList indexes = treeView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }
    
    QModelIndex index = indexes.first();
    if (index.column() != 0) {
        index = model->index(index.row(), 0, index.parent());
    }
    
    ArchiveEntry entry = model->getEntry(index);
    if (!entry.path.isEmpty()) {
        QString info = tr("File Properties\n\n"
                         "Name: %1\n"
                         "Path: %2\n"
                         "Size: %3\n"
                         "Compressed: %4\n"
                         "Date: %5\n"
                         "Type: %6")
                      .arg(QFileInfo(entry.name).fileName())
                      .arg(entry.path)
                      .arg(ArchiveUtils::formatFileSizeString(entry.size))
                      .arg(entry.compressedSize != entry.size ? 
                           ArchiveUtils::formatFileSizeString(entry.compressedSize) : tr("N/A"))
                      .arg(entry.date)
                      .arg(entry.isDirectory ? tr("Directory") : tr("File"));
        
        QMessageBox::information(this, tr("Properties"), info);
    }
}
