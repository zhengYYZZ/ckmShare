#ifndef DIALOGTCPTEST_H
#define DIALOGTCPTEST_H

#include <QDialog>
#include "tcpfileclient.h"
#include "tcpfileserver.h"
#include <QImage>

namespace Ui {
class DialogTcpTest;
}

class DialogTcpTest : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTcpTest(QWidget *parent = nullptr);
    ~DialogTcpTest();
    QByteArray get_imagedata_from_imagefile(const QImage &image);
        QImage get_imagedata_from_byte(const QString &data);

private slots:
    void tcpServerResult(QByteArray data,int type);
    void tcpServerProgress(int ReceivSzie,int fileSize);

private:
    Ui::DialogTcpTest *ui;
    TcpFileServer *tcpServer;
};

#endif // DIALOGTCPTEST_H
