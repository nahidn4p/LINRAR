#ifndef SEVENZIPHANDLER_H
#define SEVENZIPHANDLER_H

#include "../ArchiveHandler.h"

class SevenZipHandler : public ArchiveHandler {
    Q_OBJECT

public:
    explicit SevenZipHandler(QObject *parent = nullptr);
    
    bool isAvailable() const override;
    ArchiveFormat getFormat() const override { return ArchiveFormat::SevenZip; }
    
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
    
    QString getToolName() const override { return "7z"; }
    QStringList getSupportedExtensions() const override {
        return QStringList() << "7z" << "7zip";
    }

private:
    QString findSevenZipTool() const;
    bool parseListOutput(const QString &output, QList<ArchiveEntry> &entries);
};

#endif // SEVENZIPHANDLER_H
