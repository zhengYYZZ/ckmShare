#include "dialogtesttcp.h"
#include "ui_dialogtesttcp.h"

DialogTestTCP::DialogTestTCP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTestTCP)
{
    ui->setupUi(this);
    tcpClient = new TcpFileClient();
    connect(tcpClient,SIGNAL(resultSend()),this,SLOT(tcpClientResult()));
    connect(tcpClient,SIGNAL(sendDataProgress(int,int)),this,SLOT(tcpClientSendProgress(int,int)));

    tcpClient->connectServer("192.168.233.131",61618);
}

DialogTestTCP::~DialogTestTCP()
{
    delete ui;
}

void DialogTestTCP::on_pushButton_clicked()
{
    //test tcp image
    QString imgPath = "C:/Users/zyx/Pictures/wallhaven-g8ygdl.jpg";
    QImage img;
    img.load(imgPath);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(QPixmap::fromImage(img));
    QByteArray imageData = get_imagedata_from_imagefile(img);
    tcpClient->sendData(imageData,2);
}

QByteArray DialogTestTCP::get_imagedata_from_imagefile(const QImage &image)
{
    QByteArray imageData;
    QBuffer buffer(&imageData);
    image.save(&buffer, "jpg");
    imageData = imageData.toBase64();
    return imageData;
}

QImage DialogTestTCP::get_imagedata_from_byte(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}

void DialogTestTCP::tcpClientResult()
{
//    tcpClient->closeClient();
//    fileData.clear();
    qDebug()<<"\n--------发送完成------------\n";
}

void DialogTestTCP::tcpClientSendProgress(int sendSize,int fileSize)
{
    qDebug()<<"数据发送:"<<float(sendSize)/fileSize<<"%";
}
