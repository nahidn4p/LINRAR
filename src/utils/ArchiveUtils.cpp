#include "ArchiveUtils.h"
#include <QFileInfo>
#include <QDir>

QString ArchiveUtils::sanitizePath(const QString &path) {
    QString sanitized = path;
    sanitized.replace("..", "");
    sanitized.replace("//", "/");
    if (sanitized.startsWith("/")) {
        sanitized = sanitized.mid(1);
    }
    return sanitized;
}

QStringList ArchiveUtils::splitPath(const QString &path) {
    return path.split('/', Qt::SkipEmptyParts);
}

QString ArchiveUtils::joinPath(const QStringList &parts) {
    return parts.join('/');
}

bool ArchiveUtils::isValidArchiveName(const QString &name) {
    if (name.isEmpty()) {
        return false;
    }
    
    // Check for invalid characters
    QString invalidChars = "<>:\"|?*\\";
    for (QChar c : invalidChars) {
        if (name.contains(c)) {
            return false;
        }
    }
    
    return true;
}

QString ArchiveUtils::getDefaultArchiveName(const QString &basePath) {
    QFileInfo info(basePath);
    if (info.isFile()) {
        return info.baseName() + ".zip";
    } else if (info.isDir()) {
        QString dirName = info.fileName();
        if (dirName.isEmpty()) {
            dirName = QDir(basePath).dirName();
        }
        return dirName + ".zip";
    }
    return "archive.zip";
}

qint64 ArchiveUtils::formatFileSize(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return (bytes + GB / 2) / GB;
    } else if (bytes >= MB) {
        return (bytes + MB / 2) / MB;
    } else if (bytes >= KB) {
        return (bytes + KB / 2) / KB;
    }
    return bytes;
}

QString ArchiveUtils::formatFileSizeString(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;
    
    if (bytes >= TB) {
        return QString::number(bytes / (double)TB, 'f', 2) + " TB";
    } else if (bytes >= GB) {
        return QString::number(bytes / (double)GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / (double)MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / (double)KB, 'f', 2) + " KB";
    }
    return QString::number(bytes) + " B";
}
