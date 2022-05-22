#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QDebug>

connectDialog::connectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connectDialog)
{
    ui->setupUi(this);
}

connectDialog::~connectDialog()
{
    delete ui;
}

void connectDialog::on_buttonBox_accepted()
{
    m_ip = ui->ip_lineEdit->text();
    m_port = ui->port_lineEdit->text().toInt();
    m_isOk = true;
    this->close();
}

void connectDialog::on_buttonBox_rejected()
{
    this->close();
}

QString connectDialog::get_ip()
{
    return m_ip;
}

int connectDialog::get_port()
{
    return m_port;
}

bool connectDialog::is_ok()
{
    return m_isOk;

}
