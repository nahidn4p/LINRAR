#ifndef ARCHIVEVIEW_H
#define ARCHIVEVIEW_H

#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QLabel>
#include "ArchiveModel.h"
#include "ArchiveHandler.h"

class ArchiveView : public QWidget {
    Q_OBJECT

public:
    explicit ArchiveView(QWidget *parent = nullptr);
    
    void setArchive(const QString &archivePath, ArchiveHandler *handler);
    void clear();
    QStringList getSelectedFiles() const;
    QString getCurrentArchive() const { return currentArchivePath; }

signals:
    void fileSelected(const QString &filePath);
    void filesSelected(const QStringList &filePaths);
    void archiveChanged(const QString &archivePath);
    void extractRequested(const QStringList &files);
    void extractToRequested(const QStringList &files);
    void deleteRequested(const QStringList &files);
    void openWithRequested(const QString &filePath);
    void propertiesRequested(const QString &filePath);

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onSelectionChanged();
    void showContextMenu(const QPoint &pos);
    void onExtract();
    void onExtractTo();
    void onDelete();
    void onOpenWith();
    void onProperties();

private:
    void setupContextMenu();
    
    QVBoxLayout *layout;
    QLabel *archiveLabel;
    QTreeView *treeView;
    ArchiveModel *model;
    QString currentArchivePath;
    ArchiveHandler *currentHandler;
    QMenu *contextMenu;
};

#endif // ARCHIVEVIEW_H
