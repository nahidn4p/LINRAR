#include "SevenZipHandler.h"
#include "../ProcessManager.h"
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDir>

SevenZipHandler::SevenZipHandler(QObject *parent)
    : ArchiveHandler(parent) {
}

bool SevenZipHandler::isAvailable() const {
    return !findSevenZipTool().isEmpty();
}

QString SevenZipHandler::findSevenZipTool() const {
    QString sevenz = QStandardPaths::findExecutable("7z");
    if (!sevenz.isEmpty()) return sevenz;
    
    QString sevenza = QStandardPaths::findExecutable("7za");
    if (!sevenza.isEmpty()) return sevenza;
    
    return QString();
}

bool SevenZipHandler::list(const QString &archivePath, QList<ArchiveEntry> &entries) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found. Please install p7zip.");
        return false;
    }
    
    QStringList args;
    args << "l" << archivePath;
    
    QString output, errorOutput;
    if (!processManager->executeWithOutput(tool, args, output, errorOutput)) {
        emit error(processManager->getLastError());
        return false;
    }
    
    return parseListOutput(output, entries);
}

bool SevenZipHandler::parseListOutput(const QString &output, QList<ArchiveEntry> &entries) {
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    bool inFileList = false;
    
    for (const QString &line : lines) {
        if (line.contains("----") && line.contains("Name")) {
            inFileList = true;
            continue;
        }
        
        if (inFileList) {
            // 7z output format: Date Time Attr Size Compressed Name
            QRegularExpression regex(R"(^\s*(\S+\s+\S+)\s+(\S+)\s+(\d+)\s+(\d+)\s+(.+)$)");
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                ArchiveEntry entry;
                entry.name = match.captured(5).trimmed();
                entry.path = entry.name;
                entry.size = match.captured(3).toLongLong();
                entry.compressedSize = match.captured(4).toLongLong();
                entry.isDirectory = match.captured(2).startsWith('D');
                entry.date = match.captured(1);
                entries.append(entry);
            }
        }
    }
    
    return true;
}

bool SevenZipHandler::extract(const QString &archivePath, const QString &destination,
                             const QStringList &files) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found");
        return false;
    }
    
    QDir().mkpath(destination);
    
    QStringList args;
    args << "x" << archivePath << "-o" + destination;
    
    if (!files.isEmpty()) {
        args << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool SevenZipHandler::extractTo(const QString &archivePath, const QString &destination) {
    return extract(archivePath, destination);
}

bool SevenZipHandler::create(const QString &archivePath, const QStringList &files,
                            const QString &password, int compressionLevel) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found");
        return false;
    }
    
    QStringList args;
    args << "a" << "-t7z";
    
    if (!password.isEmpty()) {
        args << "-p" + password;
    }
    
    args << "-mx=" + QString::number(compressionLevel);
    args << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool SevenZipHandler::addFiles(const QString &archivePath, const QStringList &files) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found");
        return false;
    }
    
    QStringList args;
    args << "a" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool SevenZipHandler::removeFiles(const QString &archivePath, const QStringList &files) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found");
        return false;
    }
    
    QStringList args;
    args << "d" << archivePath << files;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool SevenZipHandler::test(const QString &archivePath) {
    QString tool = findSevenZipTool();
    if (tool.isEmpty()) {
        emit error("7z tool not found");
        return false;
    }
    
    QStringList args;
    args << "t" << archivePath;
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool SevenZipHandler::repair(const QString &archivePath) {
    // 7z doesn't have a repair command, but we can try to extract and recreate
    emit error("7z format does not support repair. Try extracting and recreating the archive.");
    return false;
}
