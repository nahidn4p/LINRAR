#ifndef TARHANDLER_H
#define TARHANDLER_H

#include "../ArchiveHandler.h"

class TarHandler : public ArchiveHandler {
    Q_OBJECT

public:
    explicit TarHandler(QObject *parent = nullptr);
    
    bool isAvailable() const override;
    ArchiveFormat getFormat() const override;
    
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
    
    QString getToolName() const override { return "tar"; }
    QStringList getSupportedExtensions() const override {
        return QStringList() << "tar" << "tar.gz" << "tgz" << "tar.bz2" << "tbz2" << "tar.xz" << "txz";
    }

private:
    QString findTarTool() const;
    QString getCompressionFlag(const QString &archivePath) const;
    bool parseListOutput(const QString &output, QList<ArchiveEntry> &entries);
    ArchiveFormat detectTarFormat(const QString &archivePath) const;
};

#endif // TARHANDLER_H
