#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    QStringList getRecentFiles() const;
    void addRecentFile(const QString &filePath);
    void clearRecentFiles();
    
    QString getLastOpenDirectory() const;
    void setLastOpenDirectory(const QString &path);
    
    QString getLastExtractDirectory() const;
    void setLastExtractDirectory(const QString &path);
    
    int getDefaultCompressionLevel() const;
    void setDefaultCompressionLevel(int level);
    
    bool getShowHiddenFiles() const;
    void setShowHiddenFiles(bool show);
    
    QByteArray getWindowGeometry() const;
    void setWindowGeometry(const QByteArray &geometry);
    
    QByteArray getWindowState() const;
    void setWindowState(const QByteArray &state);

private:
    QSettings *settings;
    static const int MAX_RECENT_FILES = 10;
};

#endif // SETTINGSMANAGER_H
