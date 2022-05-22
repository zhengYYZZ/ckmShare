#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class connectDialog;
}

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectDialog(QWidget *parent = nullptr);
    ~connectDialog();

    QString get_ip();
    int get_port();
    bool is_ok();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::connectDialog *ui;

    int m_port = 0;
    QString m_ip = "";
    bool m_isOk = false;
};

#endif // CONNECTDIALOG_H
