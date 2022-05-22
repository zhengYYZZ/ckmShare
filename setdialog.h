#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>

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

private:
    Ui::setDialog *ui;
};

#endif // SETDIALOG_H
