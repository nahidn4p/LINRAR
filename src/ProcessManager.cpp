#include "ProcessManager.h"
#include <QDebug>

ProcessManager::ProcessManager(QObject *parent)
    : QObject(parent), process(nullptr), lastExitCode(0) {
    process = new QProcess(this);
    
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &ProcessManager::onProcessFinished);
    connect(process, &QProcess::errorOccurred,
            this, &ProcessManager::onProcessError);
    connect(process, &QProcess::readyReadStandardOutput,
            this, &ProcessManager::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError,
            this, &ProcessManager::onReadyReadStandardError);
}

ProcessManager::~ProcessManager() {
    if (process && process->state() != QProcess::NotRunning) {
        process->kill();
        process->waitForFinished(3000);
    }
}

bool ProcessManager::execute(const QString &program, const QStringList &arguments,
                             const QString &workingDir) {
    if (isRunning()) {
        lastError = "Another process is already running";
        return false;
    }
    
    lastError.clear();
    outputBuffer.clear();
    errorBuffer.clear();
    
    if (!workingDir.isEmpty()) {
        process->setWorkingDirectory(workingDir);
    }
    
    process->start(program, arguments);
    
    if (!process->waitForStarted(5000)) {
        lastError = QString("Failed to start process: %1").arg(process->errorString());
        return false;
    }
    
    return true;
}

bool ProcessManager::executeWithOutput(const QString &program, const QStringList &arguments,
                                       QString &output, QString &error,
                                       const QString &workingDir) {
    if (!execute(program, arguments, workingDir)) {
        return false;
    }
    
    if (!process->waitForFinished(-1)) {
        lastError = "Process did not finish";
        return false;
    }
    
    output = outputBuffer;
    error = errorBuffer;
    
    return lastExitCode == 0;
}

void ProcessManager::cancel() {
    if (process && isRunning()) {
        process->kill();
        process->waitForFinished(3000);
    }
}

bool ProcessManager::isRunning() const {
    return process && process->state() != QProcess::NotRunning;
}

void ProcessManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    lastExitCode = exitCode;
    
    if (exitStatus == QProcess::CrashExit) {
        lastError = "Process crashed";
        emit errorOccurred(lastError);
    } else {
        emit finished(exitCode);
    }
}

void ProcessManager::onProcessError(QProcess::ProcessError error) {
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Process failed to start";
            break;
        case QProcess::Crashed:
            errorMsg = "Process crashed";
            break;
        case QProcess::Timedout:
            errorMsg = "Process timed out";
            break;
        case QProcess::WriteError:
            errorMsg = "Write error";
            break;
        case QProcess::ReadError:
            errorMsg = "Read error";
            break;
        default:
            errorMsg = "Unknown error";
    }
    
    lastError = errorMsg;
    emit errorOccurred(errorMsg);
}

void ProcessManager::onReadyReadStandardOutput() {
    QByteArray data = process->readAllStandardOutput();
    QString text = QString::fromUtf8(data);
    outputBuffer += text;
    emit outputReady(text);
    
    // Try to extract progress information
    emit progressUpdate(text);
}

void ProcessManager::onReadyReadStandardError() {
    QByteArray data = process->readAllStandardError();
    QString text = QString::fromUtf8(data);
    errorBuffer += text;
    emit outputReady(text);
}
