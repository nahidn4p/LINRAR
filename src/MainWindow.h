#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "FileBrowser.h"
#include "ArchiveView.h"
#include "ArchiveHandler.h"
#include "handlers/RarHandler.h"
#include "handlers/ZipHandler.h"
#include "handlers/SevenZipHandler.h"
#include "handlers/TarHandler.h"
#include "SettingsManager.h"
#include "ProgressDialog.h"
#include "AboutDialog.h"
#include "utils/FormatDetector.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openArchive();
    void newArchive();
    void closeArchive();
    void extractArchive();
    void extractSelected();
    void addFiles();
    void removeFiles();
    void testArchive();
    void repairArchive();
    void showSettings();
    void updateRecentFiles();
    void openRecentFile();
    void refreshArchive();
    void onArchiveChanged(const QString &archivePath);
    void onProgress(const QString &message, int percentage);
    void onArchiveError(const QString &error);
    void onOperationFinished();

private:
    void setupUI();
    void setupMenus();
    void setupToolbar();
    void setupStatusBar();
    void createArchiveDialog();
    ArchiveHandler* getHandlerForFormat(ArchiveFormat format);
    ArchiveHandler* getHandlerForFile(const QString &filePath);
    void updateActions();
    
    QSplitter *splitter;
    FileBrowser *fileBrowser;
    ArchiveView *archiveView;
    
    QMenu *fileMenu;
    QMenu *archiveMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QMenu *recentMenu;
    
    QToolBar *mainToolbar;
    
    QAction *openAction;
    QAction *newAction;
    QAction *closeAction;
    QAction *extractAction;
    QAction *extractSelectedAction;
    QAction *addAction;
    QAction *removeAction;
    QAction *testAction;
    QAction *repairAction;
    QAction *exitAction;
    QAction *settingsAction;
    
    RarHandler *rarHandler;
    ZipHandler *zipHandler;
    SevenZipHandler *sevenZipHandler;
    TarHandler *tarHandler;
    
    SettingsManager *settingsManager;
    ProgressDialog *progressDialog;
    
    QString currentArchivePath;
    ArchiveHandler *currentHandler;
};

#endif // MAINWINDOW_H
