#include "ArchiveModel.h"
#include "utils/ArchiveUtils.h"
#include <QFileInfo>
#include <QDir>
#include <QIcon>

ArchiveModel::ArchiveModel(QObject *parent)
    : QAbstractItemModel(parent), rootNode(new TreeNode) {
}

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    
    TreeNode *parentNode;
    if (!parent.isValid()) {
        parentNode = rootNode;
    } else {
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    }
    
    if (row < 0 || row >= parentNode->children.size()) {
        return QModelIndex();
    }
    
    TreeNode *childNode = parentNode->children.at(row);
    return createIndex(row, column, childNode);
}

QModelIndex ArchiveModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }
    
    TreeNode *childNode = static_cast<TreeNode*>(index.internalPointer());
    TreeNode *parentNode = childNode->parent;
    
    if (parentNode == rootNode) {
        return QModelIndex();
    }
    
    TreeNode *grandParentNode = parentNode->parent;
    int row = grandParentNode->children.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

int ArchiveModel::rowCount(const QModelIndex &parent) const {
    TreeNode *parentNode;
    if (!parent.isValid()) {
        parentNode = rootNode;
    } else {
        parentNode = static_cast<TreeNode*>(parent.internalPointer());
    }
    
    return parentNode->children.size();
}

int ArchiveModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant ArchiveModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    
    TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
    const ArchiveEntry &entry = node->entry;
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Name:
                return QFileInfo(entry.name).fileName();
            case Size:
                if (entry.isDirectory) {
                    return QString();
                }
                return ArchiveUtils::formatFileSizeString(entry.size);
            case Compressed:
                if (entry.isDirectory || entry.compressedSize == entry.size) {
                    return QString();
                }
                return ArchiveUtils::formatFileSizeString(entry.compressedSize);
            case Date:
                return entry.date;
            default:
                return QVariant();
        }
    } else if (role == Qt::DecorationRole && index.column() == Name) {
        if (entry.isDirectory) {
            return QIcon::fromTheme("folder");
        }
        return QIcon::fromTheme("text-x-generic");
    } else if (role == Qt::UserRole) {
        return entry.path;
    }
    
    return QVariant();
}

QVariant ArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case Name:
                return tr("Name");
            case Size:
                return tr("Size");
            case Compressed:
                return tr("Compressed");
            case Date:
                return tr("Date");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

void ArchiveModel::setEntries(const QList<ArchiveEntry> &entries) {
    beginResetModel();
    delete rootNode;
    rootNode = new TreeNode;
    buildTree(entries);
    endResetModel();
}

void ArchiveModel::clear() {
    beginResetModel();
    delete rootNode;
    rootNode = new TreeNode;
    endResetModel();
}

ArchiveEntry ArchiveModel::getEntry(const QModelIndex &index) const {
    if (!index.isValid()) {
        return ArchiveEntry();
    }
    
    TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
    return node->entry;
}

QModelIndex ArchiveModel::findEntry(const QString &path) const {
    TreeNode *node = findNode(path);
    if (!node || node == rootNode) {
        return QModelIndex();
    }
    
    TreeNode *parentNode = node->parent;
    int row = parentNode->children.indexOf(node);
    return createIndex(row, 0, node);
}

void ArchiveModel::buildTree(const QList<ArchiveEntry> &entries) {
    for (const ArchiveEntry &entry : entries) {
        QStringList parts = entry.path.split('/', Qt::SkipEmptyParts);
        TreeNode *current = rootNode;
        
        for (int i = 0; i < parts.size(); ++i) {
            QString part = parts.at(i);
            bool isLast = (i == parts.size() - 1);
            
            TreeNode *child = nullptr;
            for (TreeNode *node : current->children) {
                if (QFileInfo(node->entry.name).fileName() == part) {
                    child = node;
                    break;
                }
            }
            
            if (!child) {
                child = new TreeNode;
                child->parent = current;
                child->entry.name = part;
                child->entry.path = parts.mid(0, i + 1).join('/');
                child->entry.isDirectory = !isLast || entry.isDirectory;
                current->children.append(child);
            }
            
            if (isLast) {
                child->entry = entry;
            }
            
            current = child;
        }
    }
}

ArchiveModel::TreeNode *ArchiveModel::findNode(const QString &path) const {
    QStringList parts = path.split('/', Qt::SkipEmptyParts);
    TreeNode *current = rootNode;
    
    for (const QString &part : parts) {
        bool found = false;
        for (TreeNode *node : current->children) {
            if (QFileInfo(node->entry.name).fileName() == part) {
                current = node;
                found = true;
                break;
            }
        }
        if (!found) {
            return nullptr;
        }
    }
    
    return current;
}
