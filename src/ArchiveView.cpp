#include "ArchiveView.h"
#include <QHeaderView>
#include <QFileInfo>
#include <QMessageBox>

ArchiveView::ArchiveView(QWidget *parent)
    : QWidget(parent), currentHandler(nullptr) {
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
    
    treeView->header()->setStretchLastSection(false);
    treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    
    connect(treeView, &QTreeView::doubleClicked, this, &ArchiveView::onItemDoubleClicked);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &ArchiveView::onSelectionChanged);
    
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
