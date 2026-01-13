#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include "ArchiveHandler.h"

class ArchiveModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit ArchiveModel(QObject *parent = nullptr);
    
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void setEntries(const QList<ArchiveEntry> &entries);
    void clear();
    ArchiveEntry getEntry(const QModelIndex &index) const;
    QModelIndex findEntry(const QString &path) const;

private:
    struct TreeNode {
        ArchiveEntry entry;
        TreeNode *parent;
        QList<TreeNode*> children;
        
        TreeNode() : parent(nullptr) {}
        ~TreeNode() {
            qDeleteAll(children);
        }
    };
    
    void buildTree(const QList<ArchiveEntry> &entries);
    TreeNode *findNode(const QString &path) const;
    TreeNode *rootNode;
    
    enum Columns {
        Name = 0,
        Size,
        Compressed,
        Date,
        ColumnCount
    };
};

#endif // ARCHIVEMODEL_H
