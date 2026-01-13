#ifndef ARCHIVEUTILS_H
#define ARCHIVEUTILS_H

#include <QString>
#include <QStringList>

class ArchiveUtils {
public:
    static QString sanitizePath(const QString &path);
    static QStringList splitPath(const QString &path);
    static QString joinPath(const QStringList &parts);
    static bool isValidArchiveName(const QString &name);
    static QString getDefaultArchiveName(const QString &basePath);
    static qint64 formatFileSize(qint64 bytes);
};

#endif // ARCHIVEUTILS_H
