#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

class ProcessManager : public QObject {
    Q_OBJECT

public:
    explicit ProcessManager(QObject *parent = nullptr);
    ~ProcessManager();

    bool execute(const QString &program, const QStringList &arguments, 
                 const QString &workingDir = QString());
    bool executeWithOutput(const QString &program, const QStringList &arguments,
                          QString &output, QString &error, 
                          const QString &workingDir = QString());
    
    void cancel();
    bool isRunning() const;
    
    QString getLastError() const { return lastError; }
    int getLastExitCode() const { return lastExitCode; }
    QString getLastProgram() const { return lastProgram; }
    QString getLastOutput() const { return outputBuffer; }
    QString getLastErrorOutput() const { return errorBuffer; }

signals:
    void finished(int exitCode);
    void errorOccurred(const QString &error);
    void outputReady(const QString &output);
    void progressUpdate(const QString &message);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    QProcess *process;
    QString lastError;
    int lastExitCode;
    QString lastProgram;
    QString outputBuffer;
    QString errorBuffer;
};

#endif // PROCESSMANAGER_H
