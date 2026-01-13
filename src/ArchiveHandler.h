#ifndef ARCHIVEHANDLER_H
#define ARCHIVEHANDLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include "utils/FormatDetector.h"

class ProcessManager;

struct ArchiveEntry {
    QString name;
    QString path;
    qint64 size;
    qint64 compressedSize;
    bool isDirectory;
    QString permissions;
    QString date;
};

class ArchiveHandler : public QObject {
    Q_OBJECT

public:
    explicit ArchiveHandler(QObject *parent = nullptr);
    virtual ~ArchiveHandler() = default;

    virtual bool isAvailable() const = 0;
    virtual ArchiveFormat getFormat() const = 0;
    
    virtual bool list(const QString &archivePath, QList<ArchiveEntry> &entries) = 0;
    virtual bool extract(const QString &archivePath, const QString &destination,
                        const QStringList &files = QStringList()) = 0;
    virtual bool extractTo(const QString &archivePath, const QString &destination) = 0;
    virtual bool create(const QString &archivePath, const QStringList &files,
                       const QString &password = QString(), int compressionLevel = 5) = 0;
    virtual bool addFiles(const QString &archivePath, const QStringList &files) = 0;
    virtual bool removeFiles(const QString &archivePath, const QStringList &files) = 0;
    virtual bool test(const QString &archivePath) = 0;
    virtual bool repair(const QString &archivePath) = 0;
    
    virtual QString getToolName() const = 0;
    virtual QStringList getSupportedExtensions() const = 0;

signals:
    void progress(const QString &message, int percentage);
    void error(const QString &error);

protected:
    ProcessManager *processManager;
    bool checkToolAvailable(const QString &toolName) const;
};

#endif // ARCHIVEHANDLER_H
