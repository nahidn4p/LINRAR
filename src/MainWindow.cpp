#include "MainWindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentHandler(nullptr) {
    setWindowTitle("LINRAR - Linux Archive Manager");
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    // Initialize handlers
    rarHandler = new RarHandler(this);
    zipHandler = new ZipHandler(this);
    sevenZipHandler = new SevenZipHandler(this);
    tarHandler = new TarHandler(this);
    
    // Initialize settings
    settingsManager = new SettingsManager(this);
    
    setupUI();
    setupMenus();
    setupToolbar();
    setupStatusBar();
    
    // Restore window state
    QByteArray geometry = settingsManager->getWindowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    QByteArray state = settingsManager->getWindowState();
    if (!state.isEmpty()) {
        restoreState(state);
    }
    
    updateRecentFiles();
    updateActions();
}

MainWindow::~MainWindow() {
    // Save window state
    settingsManager->setWindowGeometry(saveGeometry());
    settingsManager->setWindowState(saveState());
}

void MainWindow::setupUI() {
    // Create splitter
    splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);
    
    // File browser (left pane)
    fileBrowser = new FileBrowser(this);
    fileBrowser->setShowHiddenFiles(settingsManager->getShowHiddenFiles());
    splitter->addWidget(fileBrowser);
    
    // Archive view (right pane)
    archiveView = new ArchiveView(this);
    splitter->addWidget(archiveView);
    
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({400, 400});
    
    connect(archiveView, &ArchiveView::archiveChanged, this, &MainWindow::onArchiveChanged);
}

void MainWindow::setupMenus() {
    // File menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    
    openAction = fileMenu->addAction(tr("&Open Archive..."), this, &MainWindow::openArchive);
    openAction->setShortcut(QKeySequence::Open);
    
    newAction = fileMenu->addAction(tr("&New Archive..."), this, &MainWindow::newArchive);
    newAction->setShortcut(QKeySequence::New);
    
    fileMenu->addSeparator();
    
    recentMenu = fileMenu->addMenu(tr("&Recent Files"));
    updateRecentFiles();
    
    fileMenu->addSeparator();
    
    closeAction = fileMenu->addAction(tr("&Close Archive"), this, &MainWindow::closeArchive);
    closeAction->setEnabled(false);
    
    fileMenu->addSeparator();
    
    exitAction = fileMenu->addAction(tr("E&xit"), qApp, &QApplication::quit);
    exitAction->setShortcut(QKeySequence::Quit);
    
    // Archive menu
    archiveMenu = menuBar()->addMenu(tr("&Archive"));
    
    extractAction = archiveMenu->addAction(tr("&Extract..."), this, &MainWindow::extractArchive);
    extractAction->setShortcut(Qt::CTRL | Qt::Key_E);
    
    extractSelectedAction = archiveMenu->addAction(tr("Extract &Selected..."), 
                                                   this, &MainWindow::extractSelected);
    
    archiveMenu->addSeparator();
    
    addAction = archiveMenu->addAction(tr("&Add Files..."), this, &MainWindow::addFiles);
    addAction->setShortcut(Qt::CTRL | Qt::Key_A);
    
    removeAction = archiveMenu->addAction(tr("&Remove Files"), this, &MainWindow::removeFiles);
    removeAction->setShortcut(Qt::Key_Delete);
    
    archiveMenu->addSeparator();
    
    testAction = archiveMenu->addAction(tr("&Test Archive"), this, &MainWindow::testArchive);
    repairAction = archiveMenu->addAction(tr("&Repair Archive"), this, &MainWindow::repairArchive);
    
    // Tools menu
    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    settingsAction = toolsMenu->addAction(tr("&Settings..."), this, &MainWindow::showSettings);
    
    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, [this]() {
        QMessageBox::about(this, tr("About LINRAR"),
            tr("LINRAR - Linux Archive Manager\n\n"
               "A modern Qt 6-based archive manager for Linux.\n\n"
               "Version 1.0.0"));
    });
}

void MainWindow::setupToolbar() {
    mainToolbar = addToolBar(tr("Main Toolbar"));
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    mainToolbar->addAction(openAction);
    mainToolbar->addAction(newAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(extractAction);
    mainToolbar->addAction(addAction);
    mainToolbar->addAction(removeAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(testAction);
}

void MainWindow::setupStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::openArchive() {
    QString lastDir = settingsManager->getLastOpenDirectory();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Archive"),
                                                    lastDir,
                                                    tr("Archives (*.rar *.zip *.7z *.tar *.tar.gz *.tar.bz2 *.tar.xz);;All Files (*)"));
    
    if (!fileName.isEmpty()) {
        settingsManager->setLastOpenDirectory(QFileInfo(fileName).absolutePath());
        settingsManager->addRecentFile(fileName);
        updateRecentFiles();
        
        ArchiveHandler *handler = getHandlerForFile(fileName);
        if (handler) {
            archiveView->setArchive(fileName, handler);
            currentArchivePath = fileName;
            currentHandler = handler;
            updateActions();
            statusBar()->showMessage(tr("Opened: %1").arg(QFileInfo(fileName).fileName()));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Unsupported archive format."));
        }
    }
}

void MainWindow::newArchive() {
    createArchiveDialog();
}

void MainWindow::createArchiveDialog() {
    QStringList files = fileBrowser->getSelectedFiles();
    if (files.isEmpty()) {
        QMessageBox::information(this, tr("No Selection"), 
                                 tr("Please select files to add to the archive."));
        return;
    }
    
    QString lastDir = settingsManager->getLastOpenDirectory();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create Archive"),
                                                    lastDir + "/archive.zip",
                                                    tr("ZIP (*.zip);;RAR (*.rar);;7Z (*.7z);;TAR (*.tar);;TAR.GZ (*.tar.gz);;TAR.BZ2 (*.tar.bz2);;All Files (*)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    settingsManager->setLastOpenDirectory(QFileInfo(fileName).absolutePath());
    
    // Ask for password (optional)
    bool ok;
    QString password = QInputDialog::getText(this, tr("Password Protection"),
                                            tr("Enter password (leave empty for no password):"),
                                            QLineEdit::Password, "", &ok);
    if (!ok) {
        return;
    }
    
    ArchiveFormat format = FormatDetector::detectFormat(fileName);
    ArchiveHandler *handler = getHandlerForFormat(format);
    
    if (!handler || !handler->isAvailable()) {
        QMessageBox::warning(this, tr("Error"), 
                            tr("Archive format not supported or tool not available."));
        return;
    }
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Creating archive..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(handler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(handler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    connect(handler, &ArchiveHandler::progress, this, &MainWindow::onProgress);
    
    // Execute in background
    QTimer::singleShot(100, [this, handler, fileName, files, password]() {
        bool success = handler->create(fileName, files, password, 
                                       settingsManager->getDefaultCompressionLevel());
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Archive created successfully."));
            archiveView->setArchive(fileName, handler);
            currentArchivePath = fileName;
            currentHandler = handler;
            updateActions();
            statusBar()->showMessage(tr("Created: %1").arg(QFileInfo(fileName).fileName()));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to create archive."));
        }
    });
}

void MainWindow::closeArchive() {
    archiveView->clear();
    currentArchivePath.clear();
    currentHandler = nullptr;
    updateActions();
    statusBar()->showMessage(tr("Archive closed"));
}

void MainWindow::extractArchive() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    QString destDir = QFileDialog::getExistingDirectory(this, tr("Extract To"),
                                                       settingsManager->getLastExtractDirectory());
    
    if (destDir.isEmpty()) {
        return;
    }
    
    settingsManager->setLastExtractDirectory(destDir);
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Extracting archive..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this, destDir]() {
        bool success = currentHandler->extractTo(currentArchivePath, destDir);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Archive extracted successfully."));
            statusBar()->showMessage(tr("Extracted to: %1").arg(destDir));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to extract archive."));
        }
    });
}

void MainWindow::extractSelected() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    QStringList files = archiveView->getSelectedFiles();
    if (files.isEmpty()) {
        QMessageBox::information(this, tr("No Selection"), 
                                tr("Please select files to extract."));
        return;
    }
    
    QString destDir = QFileDialog::getExistingDirectory(this, tr("Extract To"),
                                                       settingsManager->getLastExtractDirectory());
    
    if (destDir.isEmpty()) {
        return;
    }
    
    settingsManager->setLastExtractDirectory(destDir);
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Extracting selected files..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this, destDir, files]() {
        bool success = currentHandler->extract(currentArchivePath, destDir, files);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Files extracted successfully."));
            statusBar()->showMessage(tr("Extracted to: %1").arg(destDir));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to extract files."));
        }
    });
}

void MainWindow::addFiles() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    QStringList files = fileBrowser->getSelectedFiles();
    if (files.isEmpty()) {
        QMessageBox::information(this, tr("No Selection"), 
                                tr("Please select files to add."));
        return;
    }
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Adding files..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this, files]() {
        bool success = currentHandler->addFiles(currentArchivePath, files);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            // Refresh archive view
            archiveView->setArchive(currentArchivePath, currentHandler);
            QMessageBox::information(this, tr("Success"), tr("Files added successfully."));
            statusBar()->showMessage(tr("Files added"));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to add files."));
        }
    });
}

void MainWindow::removeFiles() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    QStringList files = archiveView->getSelectedFiles();
    if (files.isEmpty()) {
        QMessageBox::information(this, tr("No Selection"), 
                                tr("Please select files to remove."));
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm Removal"),
                                    tr("Are you sure you want to remove the selected files?"),
                                    QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Removing files..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this, files]() {
        bool success = currentHandler->removeFiles(currentArchivePath, files);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            // Refresh archive view
            archiveView->setArchive(currentArchivePath, currentHandler);
            QMessageBox::information(this, tr("Success"), tr("Files removed successfully."));
            statusBar()->showMessage(tr("Files removed"));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to remove files."));
        }
    });
}

void MainWindow::testArchive() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Testing archive..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this]() {
        bool success = currentHandler->test(currentArchivePath);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            QMessageBox::information(this, tr("Test Result"), tr("Archive is valid."));
            statusBar()->showMessage(tr("Archive test passed"));
        } else {
            QMessageBox::warning(this, tr("Test Result"), tr("Archive test failed."));
        }
    });
}

void MainWindow::repairArchive() {
    if (currentArchivePath.isEmpty() || !currentHandler) {
        return;
    }
    
    int ret = QMessageBox::question(this, tr("Confirm Repair"),
                                    tr("Repair may not be supported for all formats. Continue?"),
                                    QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    progressDialog = new ProgressDialog(this);
    progressDialog->setMessage(tr("Repairing archive..."));
    progressDialog->setIndeterminate(true);
    progressDialog->show();
    
    connect(currentHandler, &ArchiveHandler::progress, progressDialog, &ProgressDialog::setMessage);
    connect(currentHandler, &ArchiveHandler::error, this, &MainWindow::onArchiveError);
    
    QTimer::singleShot(100, [this]() {
        bool success = currentHandler->repair(currentArchivePath);
        progressDialog->close();
        progressDialog->deleteLater();
        
        if (success) {
            QMessageBox::information(this, tr("Success"), tr("Archive repaired successfully."));
            statusBar()->showMessage(tr("Archive repaired"));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to repair archive."));
        }
    });
}

void MainWindow::showSettings() {
    // Simple settings dialog
    bool ok;
    int level = QInputDialog::getInt(this, tr("Settings"),
                                    tr("Default Compression Level (0-9):"),
                                    settingsManager->getDefaultCompressionLevel(),
                                    0, 9, 1, &ok);
    if (ok) {
        settingsManager->setDefaultCompressionLevel(level);
    }
    
    bool showHidden = QMessageBox::question(this, tr("Settings"),
                                            tr("Show hidden files?"),
                                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
    settingsManager->setShowHiddenFiles(showHidden);
    fileBrowser->setShowHiddenFiles(showHidden);
}

void MainWindow::updateRecentFiles() {
    recentMenu->clear();
    QStringList recent = settingsManager->getRecentFiles();
    
    if (recent.isEmpty()) {
        recentMenu->addAction(tr("No recent files"))->setEnabled(false);
    } else {
        for (const QString &file : recent) {
            QAction *action = recentMenu->addAction(QFileInfo(file).fileName());
            action->setData(file);
            connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
        }
        recentMenu->addSeparator();
        recentMenu->addAction(tr("Clear Recent Files"), this, [this]() {
            settingsManager->clearRecentFiles();
            updateRecentFiles();
        });
    }
}

void MainWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        QString fileName = action->data().toString();
        if (QFileInfo::exists(fileName)) {
            ArchiveHandler *handler = getHandlerForFile(fileName);
            if (handler) {
                archiveView->setArchive(fileName, handler);
                currentArchivePath = fileName;
                currentHandler = handler;
                updateActions();
                statusBar()->showMessage(tr("Opened: %1").arg(QFileInfo(fileName).fileName()));
            }
        } else {
            QMessageBox::warning(this, tr("File Not Found"), 
                                tr("The file %1 no longer exists.").arg(fileName));
            settingsManager->addRecentFile(fileName); // Remove from recent
            updateRecentFiles();
        }
    }
}

void MainWindow::onArchiveChanged(const QString &archivePath) {
    Q_UNUSED(archivePath);
    updateActions();
}

void MainWindow::onProgress(const QString &message, int percentage) {
    Q_UNUSED(percentage);
    if (progressDialog) {
        progressDialog->setMessage(message);
    }
}

void MainWindow::onArchiveError(const QString &error) {
    QMessageBox::warning(this, tr("Error"), error);
    if (progressDialog) {
        progressDialog->close();
    }
}

void MainWindow::onOperationFinished() {
    if (progressDialog) {
        progressDialog->close();
    }
}

ArchiveHandler* MainWindow::getHandlerForFormat(ArchiveFormat format) {
    switch (format) {
        case ArchiveFormat::RAR:
            return rarHandler;
        case ArchiveFormat::ZIP:
            return zipHandler;
        case ArchiveFormat::SevenZip:
            return sevenZipHandler;
        case ArchiveFormat::Tar:
        case ArchiveFormat::TarGz:
        case ArchiveFormat::TarBz2:
        case ArchiveFormat::TarXz:
            return tarHandler;
        default:
            return nullptr;
    }
}

ArchiveHandler* MainWindow::getHandlerForFile(const QString &filePath) {
    ArchiveFormat format = FormatDetector::detectFormat(filePath);
    return getHandlerForFormat(format);
}

void MainWindow::updateActions() {
    bool hasArchive = !currentArchivePath.isEmpty() && currentHandler != nullptr;
    
    closeAction->setEnabled(hasArchive);
    extractAction->setEnabled(hasArchive);
    extractSelectedAction->setEnabled(hasArchive);
    addAction->setEnabled(hasArchive);
    removeAction->setEnabled(hasArchive);
    testAction->setEnabled(hasArchive);
    repairAction->setEnabled(hasArchive);
}
