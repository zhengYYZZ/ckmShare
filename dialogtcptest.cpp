#include "dialogtcptest.h"
#include "ui_dialogtcptest.h"

DialogTcpTest::DialogTcpTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTcpTest)
{
    ui->setupUi(this);
    tcpServer = new TcpFileServer();
    connect(tcpServer,SIGNAL(resultData(QByteArray,int)),this,SLOT(tcpServerResult(QByteArray,int)));
    connect(tcpServer,SIGNAL(ReceivingDataProgress(int,int)),this,SLOT(tcpServerProgress(int,int)));
    tcpServer->startListen("192.168.233.131",61618);
}

DialogTcpTest::~DialogTcpTest()
{
    delete ui;
}

void DialogTcpTest::tcpServerResult(QByteArray data,int type)
{
    if(type == 2)
    {
        //图片

        QImage img = get_imagedata_from_byte(data);
        ui->label->setScaledContents(true);
        ui->label->setPixmap(QPixmap::fromImage(img));
    }else if(type == 3)
    {
        //文件
    }
}

void DialogTcpTest::tcpServerProgress(int ReceivSzie,int fileSize)
{
    qDebug()<<"数据接收:"<<float(ReceivSzie)/fileSize<<"%";
}

QByteArray DialogTcpTest::get_imagedata_from_imagefile(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "jpg");
    imageData = imageData.toBase64();
    return imageData;
}

QImage DialogTcpTest::get_imagedata_from_byte(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}
