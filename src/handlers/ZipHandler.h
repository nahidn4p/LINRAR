#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H

#include "../ArchiveHandler.h"

class ZipHandler : public ArchiveHandler {
    Q_OBJECT

public:
    explicit ZipHandler(QObject *parent = nullptr);
    
    bool isAvailable() const override;
    ArchiveFormat getFormat() const override { return ArchiveFormat::ZIP; }
    
    bool list(const QString &archivePath, QList<ArchiveEntry> &entries) override;
    bool extract(const QString &archivePath, const QString &destination,
                const QStringList &files = QStringList()) override;
    bool extractTo(const QString &archivePath, const QString &destination) override;
    bool create(const QString &archivePath, const QStringList &files,
               const QString &password = QString(), int compressionLevel = 5) override;
    bool addFiles(const QString &archivePath, const QStringList &files) override;
    bool removeFiles(const QString &archivePath, const QStringList &files) override;
    bool test(const QString &archivePath) override;
    bool repair(const QString &archivePath) override;
    
    QString getToolName() const override { return "zip"; }
    QStringList getSupportedExtensions() const override {
        return QStringList() << "zip" << "jar" << "war";
    }

private:
    QString findZipTool() const;
    QString findUnzipTool() const;
    bool parseListOutput(const QString &output, QList<ArchiveEntry> &entries);
};

#endif // ZIPHANDLER_H
