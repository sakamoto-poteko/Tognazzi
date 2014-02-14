#include "pixelsingle.h"
#include "ui_pixelsingle.h"
#include "QPainter"
//#include "QPaintEvent"
#include "QRegion"

pixelSingle::pixelSingle(QWidget *parent, int posX, int posY, bool isActive) :
    QWidget(parent),
    ui(new Ui::pixelSingle)
{
    ui->setupUi(this);
    this->setGeometry(posX, posY, 16, 16);

    QPalette palette;

    if (isActive)
    {
        QBrush brush7(QColor(0,255,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        ui->theBotton->setEnabled(true);
    }
    else
    {
        QBrush brush7(QColor(255,0,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        ui->theBotton->setEnabled(false);
    }
    this->show();
    isSelected_ = false;
}

pixelSingle::~pixelSingle()
{
    delete ui;
}

void pixelSingle::paint(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
//        painter.setFont(QFont("Arial", 30));
    painter.drawRect(0,0,16,16);
}

void pixelSingle::on_theBotton_clicked()
{
    QPalette palette;

    if (!isSelected_)
    {
        QBrush brush7(QColor(0,0,255,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        isSelected_ = true;

        emit hasBeenChecked();
    }
    else
    {
        QBrush brush7(QColor(0,255,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
//        this->set
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        isSelected_ = false;
    }
}

void pixelSingle::setSelected (bool selected)
{
    QPalette palette;

    if (selected == true)
    {
        QBrush brush7(QColor(0,0,255,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        isSelected_ = true;

//        emit hasBeenChecked();
    }
    else
    {
        QBrush brush7(QColor(0,255,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
//        this->set
        ui->theBotton->setPalette(palette);
        ui->theBotton->setAutoFillBackground(true);

        isSelected_ = false;
    }
}
