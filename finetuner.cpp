#include "finetuner.h"
#include "ui_finetuner.h"

#include <sstream>

fineTuner::fineTuner(QWidget *parent, int initX, int initY) :
    QMainWindow(parent),
    ui(new Ui::fineTuner)
{
    ui->setupUi(this);

    ui->xPosD->setRange(0, 255);
    ui->xPosD->setValue(initX);
    ui->xPosLE->setText((QString)initX);
    currentX_ = initX;
    ui->yPosD->setRange(0, 255);
    ui->yPosD->setValue(initY);
    ui->yPosLE->setText((QString)initY);
    currentY_ = initY;
}

fineTuner::~fineTuner()
{
    delete ui;
}

void fineTuner::on_xPosD_valueChanged(int value)
{
    std::stringstream ss;
    ss << value;
    ui->xPosLE->setText((QString)ss.str().c_str());
    currentX_ = value;
    superimposeCanvas();
}

void fineTuner::on_yPosD_valueChanged(int value)
{
    std::stringstream ss;
    ss << value;
    ui->yPosLE->setText((QString)ss.str().c_str());
    currentY_ = value;
    superimposeCanvas();
}
