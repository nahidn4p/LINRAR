#include "TarHandler.h"
#include "../ProcessManager.h"
#include "../utils/FormatDetector.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDir>

TarHandler::TarHandler(QObject *parent)
    : ArchiveHandler(parent) {
}

bool TarHandler::isAvailable() const {
    return !findTarTool().isEmpty();
}

ArchiveFormat TarHandler::getFormat() const {
    // This will be set per-archive
    return ArchiveFormat::Tar;
}

ArchiveFormat TarHandler::detectTarFormat(const QString &archivePath) const {
    QFileInfo info(archivePath);
    QString ext = info.suffix().toLower();
    QString baseName = info.completeBaseName();
    
    if (ext == "tar") {
        return ArchiveFormat::Tar;
    } else if (ext == "gz" && baseName.endsWith(".tar")) {
        return ArchiveFormat::TarGz;
    } else if (ext == "bz2" && baseName.endsWith(".tar")) {
        return ArchiveFormat::TarBz2;
    } else if (ext == "xz" && baseName.endsWith(".tar")) {
        return ArchiveFormat::TarXz;
    } else if (ext == "tgz") {
        return ArchiveFormat::TarGz;
    } else if (ext == "tbz2") {
        return ArchiveFormat::TarBz2;
    } else if (ext == "txz") {
        return ArchiveFormat::TarXz;
    }
    
    return ArchiveFormat::Tar;
}

QString TarHandler::getCompressionFlag(const QString &archivePath) const {
    ArchiveFormat format = detectTarFormat(archivePath);
    
    switch (format) {
        case ArchiveFormat::TarGz:
            return "z";
        case ArchiveFormat::TarBz2:
            return "j";
        case ArchiveFormat::TarXz:
            return "J";
        default:
            return "";
    }
}

bool TarHandler::list(const QString &archivePath, QList<ArchiveEntry> &entries) {
    QString tool = findTarTool();
    if (tool.isEmpty()) {
        emit error("tar tool not found");
        return false;
    }
    
    QString compFlag = getCompressionFlag(archivePath);
    QStringList args;
    args << "-t" + compFlag + "v" << "-f" << archivePath;
    
    QString output, errorOutput;
    if (!processManager->executeWithOutput(tool, args, output, errorOutput)) {
        emit error(processManager->getLastError());
        return false;
    }
    
    return parseListOutput(output, entries);
}

bool TarHandler::parseListOutput(const QString &output, QList<ArchiveEntry> &entries) {
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    
    // tar -tv output: permissions size date time name
    QRegularExpression regex(R"(^([rwx-]+)\s+(\S+)\s+(\S+)\s+(\S+\s+\S+)\s+(.+)$)");
    
    for (const QString &line : lines) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            ArchiveEntry entry;
            entry.permissions = match.captured(1);
            entry.size = match.captured(2).toLongLong();
            entry.date = match.captured(4);
            entry.name = match.captured(5).trimmed();
            entry.path = entry.name;
            entry.isDirectory = entry.permissions.startsWith('d') || entry.name.endsWith('/');
            entry.compressedSize = entry.size; // tar doesn't show compressed size separately
            entries.append(entry);
        }
    }
    
    return true;
}

bool TarHandler::extract(const QString &archivePath, const QString &destination,
                        const QStringList &files) {
    QString tool = findTarTool();
    if (tool.isEmpty()) {
        emit error("tar tool not found");
        return false;
    }
    
    QDir().mkpath(destination);
    
    QString compFlag = getCompressionFlag(archivePath);
    QStringList args;
    args << "-x" + compFlag << "-f" << archivePath << "-C" << destination;
    
    if (!files.isEmpty()) {
        args << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool TarHandler::extractTo(const QString &archivePath, const QString &destination) {
    return extract(archivePath, destination);
}

bool TarHandler::create(const QString &archivePath, const QStringList &files,
                       const QString &password, int compressionLevel) {
    // tar doesn't support passwords
    Q_UNUSED(password);
    
    QString tool = findTarTool();
    if (tool.isEmpty()) {
        emit error("tar tool not found");
        return false;
    }
    
    QString compFlag = getCompressionFlag(archivePath);
    QStringList args;
    args << "-c" + compFlag << "-f" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool TarHandler::addFiles(const QString &archivePath, const QStringList &files) {
    QString tool = findTarTool();
    if (tool.isEmpty()) {
        emit error("tar tool not found");
        return false;
    }
    
    QString compFlag = getCompressionFlag(archivePath);
    QStringList args;
    args << "-r" + compFlag << "-f" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool TarHandler::removeFiles(const QString &archivePath, const QStringList &files) {
    // tar doesn't support removing files directly
    // Would need to extract, remove, and recreate
    emit error("tar format does not support removing files. Extract, modify, and recreate the archive.");
    return false;
}

bool TarHandler::test(const QString &archivePath) {
    QString tool = findTarTool();
    if (tool.isEmpty()) {
        emit error("tar tool not found");
        return false;
    }
    
    QString compFlag = getCompressionFlag(archivePath);
    QStringList args;
    args << "-t" + compFlag << "-f" << archivePath;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool TarHandler::repair(const QString &archivePath) {
    emit error("tar format does not support repair");
    return false;
}

QString TarHandler::findTarTool() const {
    return QStandardPaths::findExecutable("tar");
}
