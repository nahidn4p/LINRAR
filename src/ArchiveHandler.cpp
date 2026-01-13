#include "ArchiveHandler.h"
#include "ProcessManager.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QProcess>

ArchiveHandler::ArchiveHandler(QObject *parent)
    : QObject(parent) {
    processManager = new ProcessManager(this);
    
    connect(processManager, &ProcessManager::progressUpdate,
            this, [this](const QString &msg) {
        emit progress(msg, -1);
    });
    
    connect(processManager, &ProcessManager::errorOccurred,
            this, &ArchiveHandler::error);
}

bool ArchiveHandler::checkToolAvailable(const QString &toolName) const {
    QString program = QStandardPaths::findExecutable(toolName);
    if (program.isEmpty()) {
        // Try with which command
        QProcess proc;
        proc.start("which", QStringList() << toolName);
        proc.waitForFinished(1000);
        return proc.exitCode() == 0;
    }
    return true;
}
