#ifndef DIALOGTESTTCP_H
#define DIALOGTESTTCP_H

#include <QDialog>
#include "tcpfileclient.h"
#include "tcpfileserver.h"
#include <QImage>
#include <QPixmap>

namespace Ui {
class DialogTestTCP;
}

class DialogTestTCP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTestTCP(QWidget *parent = nullptr);
    ~DialogTestTCP();
    QByteArray get_imagedata_from_imagefile(const QImage &image);
    QImage get_imagedata_from_byte(const QString &data);

private slots:
    void on_pushButton_clicked();
    void tcpClientResult();
    void tcpClientSendProgress(int sendSize,int fileSize);

private:
    Ui::DialogTestTCP *ui;
    TcpFileClient *tcpClient;
};

#endif // DIALOGTESTTCP_H
