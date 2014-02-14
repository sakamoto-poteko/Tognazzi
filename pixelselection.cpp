#include "pixelselection.h"
#include "ui_pixelselection.h"

#include "pixelselection.h"

pixelSelection::pixelSelection(QWidget *parent) :
//    QWidget(parent),
    QMainWindow(parent),
    fileReader(),
    ui(new Ui::pixelSelection)
{
    ui->setupUi(this);

    setWindowTitle("Select Chips to Plot");

    isOneCheckable_ = false;

//    isPixelActive_ = reader.isPixelActive_;
//    maxRows_ = reader.maxRows_;
//    maxCols_ = reader.maxCols_;
}

pixelSelection::pixelSelection(QWidget *parent, fileReader* reader, QString spc, std::string varName) :
//    QWidget(parent),
    QMainWindow(parent),
    fileReader(),
    ui(new Ui::pixelSelection)
{
    ui->setupUi(this);

    setWindowTitle("Select a Chip");

    isPixelActive_ = reader->isPixelActive_;
    maxRows_ = reader->maxRows_;
    maxCols_ = reader->maxCols_;

    for (int i = 0; i < (int)maxRows_/2; ++i)
    {
        for (int j = 0; j < (int)maxCols_/2; ++j)
        {
//            ui->tabWidget->setCurrentWidget(ui->UpLeftTab);
            if (thePixels_[std::pair<int,int>(i,j)]) delete thePixels_[std::pair<int,int>(j,i)];
            {
                thePixels_[std::pair<int,int>(i,j)] = (new pixelSingle (ui->UpperLeftW, j*16, i*16, isPixelActive_[spc][std::pair<int,int>(i,j)][varName]));
//                connect(thePixels_[std::pair<int,int>(i,j)], SIGNAL(hasBeenChecked()), this, SLOT(imposeOneSelected(pixelSingle *px)));
            }
        }
        for (int j = (int)maxCols_/2; j < maxCols_; ++j)
        {
//            ui->tabWidget->setCurrentWidget(ui->DownLeftTab);
            if (thePixels_[std::pair<int,int>(i,j)]) delete thePixels_[std::pair<int,int>(j,i)];
                thePixels_[std::pair<int,int>(i,j)] = (new pixelSingle (ui->LowerLeftW, (j-(int)maxCols_/2)*16, i*16, isPixelActive_[spc][std::pair<int,int>(i,j)][varName]));
        }
    }
    for (int i = (int)maxRows_/2; i < maxRows_; ++i)
    {
        for (int j = 0; j < (int)maxCols_/2; ++j)
        {
//            ui->tabWidget->setCurrentWidget(ui->UpRightTab);
            if (thePixels_[std::pair<int,int>(i,j)]) delete thePixels_[std::pair<int,int>(j,i)];
                thePixels_[std::pair<int,int>(i,j)] = (new pixelSingle (ui->UpperRightW, j*16, (i-(int)maxRows_/2)*16, isPixelActive_[spc][std::pair<int,int>(i,j)][varName]));
        }
        for (int j = (int)maxCols_/2; j < maxCols_; ++j)
        {
//            ui->tabWidget->setCurrentWidget(ui->DownRightTab);
            if (thePixels_[std::pair<int,int>(i,j)]) delete thePixels_[std::pair<int,int>(j,i)];
                thePixels_[std::pair<int,int>(i,j)] = (new pixelSingle (ui->LowerRightW, (j-(int)maxCols_/2)*16, (i-(int)maxRows_/2)*16, isPixelActive_[spc][std::pair<int,int>(i,j)][varName]));
        }
    }

//    ui->tabWidget->setCurrentWidget(ui->UpLeftTab);

//    QPixmap * map = new QPixmap (16*maxRows_, 16*maxCols_);
//    QPainter * painter = new QPainter();
//    painter->drawPixMap(0,0,map);

    isOneCheckable_ = false;
}

pixelSelection::~pixelSelection()
{
    delete ui;
}

void pixelSelection::on_closeButtonPB_clicked()
{
    delete this;
}

void pixelSelection::on_drawPB_clicked()
{
    if (!isOneCheckable_)
    {
        for (std::map<std::pair<int,int>, pixelSingle*>::iterator i = thePixels_.begin(); i != thePixels_.end(); ++i)
        {
            if (i->second->isSelected())
            {
                pixelsSelected_.push_back(i->first);
            }
        }
    }

    else
    {

    }

    emit drawPixels();

    pixelsSelected_.clear();

    return;
}


void pixelSelection::imposeOneSelected (pixelSingle *px)
{
    if (!isOneCheckable_)
        return;

    for (int i = 0; i < (int)maxRows_; ++i)
    {
        for (int j = 0; j < (int)maxCols_; ++j)
        {
//            ui->tabWidget->setCurrentWidget(ui->UpLeftTab);
            if (thePixels_[std::pair<int,int>(i,j)]->isSelected() && px->getRow() != i && px->getCol() != j)
                thePixels_[std::pair<int,int>(i,j)]->setSelected(false);
        }
    }
}
