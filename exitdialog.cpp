#include "exitdialog.h"
#include "ui_exitdialog.h"

ExitDialog::ExitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitDialog)
{
    ui->setupUi(this);
}

ExitDialog::~ExitDialog()
{
    delete ui;
}

void ExitDialog::on_buttonBox_accepted()
{
    is_ok = true;
}

void ExitDialog::on_buttonBox_rejected()
{
    is_ok = false;
}

void ExitDialog::on_TrayRadioButton_clicked()
{
    is_exit = false;
}

void ExitDialog::on_radioButton_2_clicked()
{
    is_exit = true;
}

bool ExitDialog::get_is_ok()
{
    return is_ok;
}

bool ExitDialog::get_is_exit()
{
    return is_exit;
}
