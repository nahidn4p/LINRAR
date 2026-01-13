#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QToolBar>

class FileBrowser : public QWidget {
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);
    
    QString getCurrentPath() const;
    void setCurrentPath(const QString &path);
    QStringList getSelectedFiles() const;
    void setShowHiddenFiles(bool show);

signals:
    void fileSelected(const QString &filePath);
    void filesSelected(const QStringList &filePaths);
    void directoryChanged(const QString &path);

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onSelectionChanged();
    void onPathChanged();
    void goUp();
    void goHome();

private:
    QVBoxLayout *layout;
    QToolBar *toolbar;
    QLineEdit *pathEdit;
    QTreeView *treeView;
    QFileSystemModel *model;
};

#endif // FILEBROWSER_H
