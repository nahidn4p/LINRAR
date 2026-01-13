#include "AboutDialog.h"
#include <QApplication>
#include <QTextEdit>
#include <QScrollArea>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("About LINRAR"));
    setMinimumSize(500, 400);
    setupUI();
}

void AboutDialog::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Application name and version
    QLabel *titleLabel = new QLabel("LINRAR", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(titleLabel);
    
    QLabel *versionLabel = new QLabel(tr("Version 1.0.0"), this);
    versionLabel->setStyleSheet("font-size: 14px; color: #666;");
    layout->addWidget(versionLabel);
    
    layout->addSpacing(10);
    
    // Description
    QLabel *descLabel = new QLabel(
        tr("Linux Archive Manager - WinRAR alternative for Linux\n\n"
           "A modern Qt-based archive manager that provides WinRAR-like\n"
           "functionality by interfacing with native command-line tools."),
        this);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);
    
    layout->addSpacing(10);
    
    // Information
    QString infoText = tr(
        "<b>Copyright</b> © 2024 LINRAR Team<br><br>"
        "<b>License:</b> MIT License<br><br>"
        "<b>Qt Version:</b> %1<br>"
        "<b>Build Date:</b> %2<br><br>"
        "<b>Features:</b><br>"
        "• Support for RAR, ZIP, 7Z, TAR formats<br>"
        "• Create, extract, view, and modify archives<br>"
        "• Password protection and encryption<br>"
        "• Modern dual-pane interface<br><br>"
        "<b>Dependencies:</b><br>"
        "Requires native tools: rar/unrar, 7z, zip/unzip, tar"
    ).arg(qVersion()).arg(__DATE__);
    
    QLabel *infoLabel = new QLabel(infoText, this);
    infoLabel->setWordWrap(true);
    infoLabel->setTextFormat(Qt::RichText);
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    
    // Close button
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);
}
