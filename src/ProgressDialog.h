#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class ProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    
    void setMessage(const QString &message);
    void setProgress(int percentage);
    void setIndeterminate(bool indeterminate);

public slots:
    void onCancel();

signals:
    void cancelled();

private:
    QLabel *messageLabel;
    QProgressBar *progressBar;
    QPushButton *cancelButton;
    bool cancelledFlag;
};

#endif // PROGRESSDIALOG_H
