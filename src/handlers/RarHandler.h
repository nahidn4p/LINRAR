#ifndef RARHANDLER_H
#define RARHANDLER_H

#include "../ArchiveHandler.h"

class RarHandler : public ArchiveHandler {
    Q_OBJECT

public:
    explicit RarHandler(QObject *parent = nullptr);
    
    bool isAvailable() const override;
    ArchiveFormat getFormat() const override { return ArchiveFormat::RAR; }
    
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
    
    QString getToolName() const override { return "rar"; }
    QStringList getSupportedExtensions() const override {
        return QStringList() << "rar" << "r00" << "r01";
    }

private:
    QString findRarTool() const;
    bool parseListOutput(const QString &output, QList<ArchiveEntry> &entries);
};

#endif // RARHANDLER_H
