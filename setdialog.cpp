#include "setdialog.h"
#include "ui_setdialog.h"
#include <QDebug>

setDialog::setDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setDialog)
{
    ui->setupUi(this);

    QSettings config("./SYSTEM.INI",QSettings::IniFormat);
    QString fileSize = config.value("/set/fileSize").toString();
    QString filePath = config.value("/set/TempFilePath").toString();
    QString port = config.value("/set/port").toString();

    ui->fileSizeLineEdit->setText(fileSize);
    ui->filePathLineEdit->setText(filePath);
    ui->PortlineEdit->setText(port);

}

setDialog::~setDialog()
{
    delete ui;
}

void setDialog::on_buttonBox_accepted()
{
    QString fileSize = ui->fileSizeLineEdit->text();
    QString port = ui->PortlineEdit->text();
    QString filePath = ui->filePathLineEdit->text();

    QSettings config("./SYSTEM.INI",QSettings::IniFormat);
    config.setValue("/set/fileSize",fileSize);
    config.setValue("/set/port",port);
    config.setValue("/set/TempFilePath",filePath);
    close();
}

void setDialog::on_buttonBox_rejected()
{
    close();
}

void setDialog::on_filePathPushButton_clicked()
{
    QString fileDirPath = QFileDialog::getExistingDirectory(this,tr("设置保存目录"));
    ui->filePathLineEdit->setText(fileDirPath);
}
