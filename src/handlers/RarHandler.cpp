#include "RarHandler.h"
#include "../ProcessManager.h"
#include <QStandardPaths>
#include <QProcess>
#include <QRegularExpression>
#include <QDir>

RarHandler::RarHandler(QObject *parent)
    : ArchiveHandler(parent) {
}

bool RarHandler::isAvailable() const {
    return !findRarTool().isEmpty();
}

QString RarHandler::findRarTool() const {
    // Try rar first (for creating), then unrar (for extracting)
    QString rar = QStandardPaths::findExecutable("rar");
    if (!rar.isEmpty()) return rar;
    
    QString unrar = QStandardPaths::findExecutable("unrar");
    if (!unrar.isEmpty()) return unrar;
    
    return QString();
}

bool RarHandler::list(const QString &archivePath, QList<ArchiveEntry> &entries) {
    QString tool = findRarTool();
    if (tool.isEmpty()) {
        emit error("RAR tool not found. Please install rar or unrar.");
        return false;
    }
    
    QString output, errorOutput;
    QStringList args;
    
    if (tool.contains("unrar")) {
        args << "l" << "-v" << archivePath;
    } else {
        args << "l" << "-v" << archivePath;
    }
    
    if (!processManager->executeWithOutput(tool, args, output, errorOutput)) {
        emit error(processManager->getLastError());
        return false;
    }
    
    return parseListOutput(output, entries);
}

bool RarHandler::parseListOutput(const QString &output, QList<ArchiveEntry> &entries) {
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    bool inFileList = false;
    
    QRegularExpression fileRegex(R"(^\s*(\S+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\S+\s+\S+)\s+(.+)$)");
    
    for (const QString &line : lines) {
        if (line.contains("Name") && line.contains("Size")) {
            inFileList = true;
            continue;
        }
        
        if (inFileList) {
            QRegularExpressionMatch match = fileRegex.match(line);
            if (match.hasMatch()) {
                ArchiveEntry entry;
                entry.name = match.captured(6).trimmed();
                entry.path = entry.name;
                entry.size = match.captured(2).toLongLong();
                entry.compressedSize = match.captured(3).toLongLong();
                entry.isDirectory = entry.name.endsWith('/');
                entry.date = match.captured(5);
                entries.append(entry);
            }
        }
    }
    
    return true;
}

bool RarHandler::extract(const QString &archivePath, const QString &destination,
                        const QStringList &files) {
    QString tool = findRarTool();
    if (tool.isEmpty()) {
        emit error("RAR tool not found");
        return false;
    }
    
    QDir().mkpath(destination);
    
    QStringList args;
    if (tool.contains("unrar")) {
        args << "x" << "-o+" << archivePath << destination + "/";
    } else {
        args << "x" << "-o+" << archivePath << destination + "/";
    }
    
    if (!files.isEmpty()) {
        args << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool RarHandler::extractTo(const QString &archivePath, const QString &destination) {
    return extract(archivePath, destination);
}

bool RarHandler::create(const QString &archivePath, const QStringList &files,
                       const QString &password, int compressionLevel) {
    QString tool = QStandardPaths::findExecutable("rar");
    if (tool.isEmpty()) {
        emit error("rar tool not found. unrar can only extract, not create archives.");
        return false;
    }
    
    QStringList args;
    args << "a";
    
    if (!password.isEmpty()) {
        args << "-p" + password;
    }
    
    args << "-m" + QString::number(compressionLevel);
    args << archivePath;
    args << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool RarHandler::addFiles(const QString &archivePath, const QStringList &files) {
    QString tool = QStandardPaths::findExecutable("rar");
    if (tool.isEmpty()) {
        emit error("rar tool not found");
        return false;
    }
    
    QStringList args;
    args << "a" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool RarHandler::removeFiles(const QString &archivePath, const QStringList &files) {
    QString tool = QStandardPaths::findExecutable("rar");
    if (tool.isEmpty()) {
        emit error("rar tool not found");
        return false;
    }
    
    QStringList args;
    args << "d" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool RarHandler::test(const QString &archivePath) {
    QString tool = findRarTool();
    if (tool.isEmpty()) {
        emit error("RAR tool not found");
        return false;
    }
    
    QStringList args;
    if (tool.contains("unrar")) {
        args << "t" << archivePath;
    } else {
        args << "t" << archivePath;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool RarHandler::repair(const QString &archivePath) {
    QString tool = QStandardPaths::findExecutable("rar");
    if (tool.isEmpty()) {
        emit error("rar tool not found");
        return false;
    }
    
    QStringList args;
    args << "r" << archivePath;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}
