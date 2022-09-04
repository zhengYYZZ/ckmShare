#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>

namespace Ui {
class ExitDialog;
}

class ExitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExitDialog(QWidget *parent = nullptr);
    ~ExitDialog();
    bool get_is_ok();
    bool get_is_exit();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_TrayRadioButton_clicked();

    void on_radioButton_2_clicked();

private:
    Ui::ExitDialog *ui;

public:
    bool is_ok = false;
    bool is_exit = false;
};

#endif // EXITDIALOG_H
