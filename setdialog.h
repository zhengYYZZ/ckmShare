#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class setDialog;
}

class setDialog : public QDialog
{
    Q_OBJECT

public:
    explicit setDialog(QWidget *parent = nullptr);
    ~setDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::setDialog *ui;
};

#endif // SETDIALOG_H
