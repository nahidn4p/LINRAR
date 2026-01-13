#include "ProgressDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent), cancelledFlag(false) {
    setWindowTitle(tr("Processing..."));
    setModal(true);
    setMinimumWidth(400);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    messageLabel = new QLabel(tr("Processing archive..."), this);
    layout->addWidget(messageLabel);
    
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    layout->addWidget(progressBar);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton(tr("Cancel"), this);
    connect(cancelButton, &QPushButton::clicked, this, &ProgressDialog::onCancel);
    buttonLayout->addWidget(cancelButton);
    
    layout->addLayout(buttonLayout);
}

void ProgressDialog::setMessage(const QString &message) {
    messageLabel->setText(message);
}

void ProgressDialog::setProgress(int percentage) {
    if (percentage >= 0 && percentage <= 100) {
        progressBar->setValue(percentage);
    }
}

void ProgressDialog::setIndeterminate(bool indeterminate) {
    if (indeterminate) {
        progressBar->setRange(0, 0);
    } else {
        progressBar->setRange(0, 100);
    }
}

void ProgressDialog::onCancel() {
    cancelledFlag = true;
    emit cancelled();
    reject();
}
