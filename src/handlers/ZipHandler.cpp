#include "ZipHandler.h"
#include "../ProcessManager.h"
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDir>

ZipHandler::ZipHandler(QObject *parent)
    : ArchiveHandler(parent) {
}

bool ZipHandler::isAvailable() const {
    return !findZipTool().isEmpty() && !findUnzipTool().isEmpty();
}

QString ZipHandler::findZipTool() const {
    QString zip = QStandardPaths::findExecutable("zip");
    if (!zip.isEmpty()) return zip;
    
    // Try 7z as fallback
    QString sevenz = QStandardPaths::findExecutable("7z");
    if (!sevenz.isEmpty()) return sevenz;
    
    return QString();
}

QString ZipHandler::findUnzipTool() const {
    QString unzip = QStandardPaths::findExecutable("unzip");
    if (!unzip.isEmpty()) return unzip;
    
    // Try 7z as fallback
    QString sevenz = QStandardPaths::findExecutable("7z");
    if (!sevenz.isEmpty()) return sevenz;
    
    return QString();
}

bool ZipHandler::list(const QString &archivePath, QList<ArchiveEntry> &entries) {
    QString tool = findUnzipTool();
    if (tool.isEmpty()) {
        emit error("unzip tool not found");
        return false;
    }
    
    QString output, errorOutput;
    QStringList args;
    
    if (tool.contains("7z")) {
        args << "l" << archivePath;
    } else {
        args << "-l" << archivePath;
    }
    
    if (!processManager->executeWithOutput(tool, args, output, errorOutput)) {
        emit error(processManager->getLastError());
        return false;
    }
    
    return parseListOutput(output, entries);
}

bool ZipHandler::parseListOutput(const QString &output, QList<ArchiveEntry> &entries) {
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    bool inFileList = false;
    
    for (const QString &line : lines) {
        if (line.contains("Length") && line.contains("Name")) {
            inFileList = true;
            continue;
        }
        
        if (inFileList && line.startsWith("------")) {
            continue;
        }
        
        if (inFileList) {
            // Parse: Length   Date   Time   Name
            QRegularExpression regex(R"(^\s*(\d+)\s+(\S+\s+\S+\s+\S+)\s+(.+)$)");
            QRegularExpressionMatch match = regex.match(line);
            if (match.hasMatch()) {
                ArchiveEntry entry;
                entry.name = match.captured(3).trimmed();
                entry.path = entry.name;
                entry.size = match.captured(1).toLongLong();
                entry.compressedSize = entry.size; // unzip -l doesn't show compressed size
                entry.isDirectory = entry.name.endsWith('/');
                entry.date = match.captured(2);
                entries.append(entry);
            }
        }
    }
    
    return true;
}

bool ZipHandler::extract(const QString &archivePath, const QString &destination,
                        const QStringList &files) {
    QString tool = findUnzipTool();
    if (tool.isEmpty()) {
        emit error("unzip tool not found");
        return false;
    }
    
    QDir().mkpath(destination);
    
    QStringList args;
    if (tool.contains("7z")) {
        args << "x" << archivePath << "-o" + destination;
        if (!files.isEmpty()) {
            args << files;
        }
    } else {
        args << "-o" + destination << archivePath;
        if (!files.isEmpty()) {
            args << files;
        }
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool ZipHandler::extractTo(const QString &archivePath, const QString &destination) {
    return extract(archivePath, destination);
}

bool ZipHandler::create(const QString &archivePath, const QStringList &files,
                       const QString &password, int compressionLevel) {
    QString tool = findZipTool();
    if (tool.isEmpty()) {
        emit error("zip tool not found");
        return false;
    }
    
    QStringList args;
    if (tool.contains("7z")) {
        args << "a" << "-tzip";
        if (!password.isEmpty()) {
            args << "-p" + password;
        }
        args << "-mx=" + QString::number(compressionLevel);
        args << archivePath << files;
    } else {
        args << "-" + QString::number(compressionLevel);
        if (!password.isEmpty()) {
            args << "-P" + password;
        }
        args << archivePath << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool ZipHandler::addFiles(const QString &archivePath, const QStringList &files) {
    QString tool = findZipTool();
    if (tool.isEmpty()) {
        emit error("zip tool not found");
        return false;
    }
    
    QStringList args;
    if (tool.contains("7z")) {
        args << "a" << "-tzip" << archivePath << files;
    } else {
        args << archivePath << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool ZipHandler::removeFiles(const QString &archivePath, const QStringList &files) {
    QString tool = findZipTool();
    if (tool.isEmpty()) {
        emit error("zip tool not found");
        return false;
    }
    
    QStringList args;
    if (tool.contains("7z")) {
        args << "d" << "-tzip" << archivePath << files;
    } else {
        args << "-d" << archivePath << files;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool ZipHandler::test(const QString &archivePath) {
    QString tool = findUnzipTool();
    if (tool.isEmpty()) {
        emit error("unzip tool not found");
        return false;
    }
    
    QStringList args;
    if (tool.contains("7z")) {
        args << "t" << archivePath;
    } else {
        args << "-t" << archivePath;
    }
    
    QString output, errorOutput;
    return processManager->executeWithOutput(tool, args, output, errorOutput);
}

bool ZipHandler::repair(const QString &archivePath) {
    // ZIP repair is limited, try zip -F
    QString tool = findZipTool();
    if (tool.isEmpty()) {
        emit error("zip tool not found");
        return false;
    }
    
    if (tool.contains("7z")) {
        emit error("7z does not support ZIP repair");
        return false;
    }
    
    QStringList args;
    args << "-F" << archivePath << "--out" << archivePath + ".fixed";
    
    QString output, errorOutput;
    bool result = processManager->executeWithOutput(tool, args, output, errorOutput);
    // Note: User would need to manually replace the file
    return result;
}
