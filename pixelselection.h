#ifndef PIXELSELECTION_H
#define PIXELSELECTION_H

#include <QWidget>
#include <QMainWindow>

#include "filereader.h"
#include "pixelsingle.h"

class MainWindow;

namespace Ui {
class pixelSelection;
}

class pixelSelection : public QMainWindow, public fileReader
{
    Q_OBJECT

    friend class MainWindow;

public:
//    explicit pixelSelection(QWidget *parent = 0, fileReader& reader);
    pixelSelection(QWidget *parent = 0);
    pixelSelection(QWidget *parent, fileReader* reader, QString spc, std::string varName);
    ~pixelSelection();

    std::vector<std::pair<int,int> > getPixelsSelected () {return pixelsSelected_;}

    void setOneCheckable (bool is) {isOneCheckable_ = is;}
    
signals:
    void drawPixels();

private slots:
    void on_closeButtonPB_clicked();

    void on_drawPB_clicked();

    void imposeOneSelected (pixelSingle * px);

private:
    Ui::pixelSelection *ui;

    bool isOneCheckable_;

    std::map<std::pair<int,int>, pixelSingle *> thePixels_;

    std::vector<std::pair<int,int> > pixelsSelected_;
};

#endif // PIXELSELECTION_H
