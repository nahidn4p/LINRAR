#include "SettingsManager.h"
#include <QStandardPaths>
#include <QDir>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent) {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString appConfigPath = configPath + "/linrar";
    QDir().mkpath(appConfigPath);
    
    settings = new QSettings(appConfigPath + "/linrar.conf", QSettings::IniFormat, this);
}

QStringList SettingsManager::getRecentFiles() const {
    return settings->value("recentFiles", QStringList()).toStringList();
}

void SettingsManager::addRecentFile(const QString &filePath) {
    QStringList recent = getRecentFiles();
    recent.removeAll(filePath);
    recent.prepend(filePath);
    
    while (recent.size() > MAX_RECENT_FILES) {
        recent.removeLast();
    }
    
    settings->setValue("recentFiles", recent);
    settings->sync();
}

void SettingsManager::clearRecentFiles() {
    settings->remove("recentFiles");
    settings->sync();
}

QString SettingsManager::getLastOpenDirectory() const {
    return settings->value("lastOpenDirectory", QDir::homePath()).toString();
}

void SettingsManager::setLastOpenDirectory(const QString &path) {
    settings->setValue("lastOpenDirectory", path);
    settings->sync();
}

QString SettingsManager::getLastExtractDirectory() const {
    return settings->value("lastExtractDirectory", QDir::homePath()).toString();
}

void SettingsManager::setLastExtractDirectory(const QString &path) {
    settings->setValue("lastExtractDirectory", path);
    settings->sync();
}

int SettingsManager::getDefaultCompressionLevel() const {
    return settings->value("defaultCompressionLevel", 5).toInt();
}

void SettingsManager::setDefaultCompressionLevel(int level) {
    settings->setValue("defaultCompressionLevel", level);
    settings->sync();
}

bool SettingsManager::getShowHiddenFiles() const {
    return settings->value("showHiddenFiles", false).toBool();
}

void SettingsManager::setShowHiddenFiles(bool show) {
    settings->setValue("showHiddenFiles", show);
    settings->sync();
}

QByteArray SettingsManager::getWindowGeometry() const {
    return settings->value("windowGeometry").toByteArray();
}

void SettingsManager::setWindowGeometry(const QByteArray &geometry) {
    settings->setValue("windowGeometry", geometry);
    settings->sync();
}

QByteArray SettingsManager::getWindowState() const {
    return settings->value("windowState").toByteArray();
}

void SettingsManager::setWindowState(const QByteArray &state) {
    settings->setValue("windowState", state);
    settings->sync();
}
