#include "pixelselectionbutton.h"

pixelSelectionButton::pixelSelectionButton(QWidget *parent, int posX, int posY, bool isActive) :
    QPushButton(parent)
//    ui(new Ui::pixelSelectionButton)
{
    this->setGeometry(posX, posY, 10, 10);

    QPalette palette;

    if (isActive)
    {
        QBrush brush7(QColor(0,255,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);

        this->setEnabled(true);
    }
    else
    {
        QBrush brush7(QColor(255,0,0,255));
        palette.setBrush(QPalette::Active, QPalette::Window, brush7);
        palette.setBrush(QPalette::Active, QPalette::Button, brush7);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush7);
        this->setAutoFillBackground(true);
        this->setPalette(palette);

        this->setEnabled(false);
    }
    this->show();
    isSelected_ = false;
}

void pixelSelectionButton::on_theBotton_clicked()
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

        isSelected_ = true;
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

        isSelected_ = false;
    }
}
