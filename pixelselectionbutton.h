#ifndef PIXELSELECTIONBUTTON_H
#define PIXELSELECTIONBUTTON_H

#include "QPushButton"
#include "qwidget.h"

class pixelSelection;

class pixelSelectionButton : public QPushButton
{
public:

    Q_OBJECT

    pixelSelectionButton(QWidget *parent = 0, int posX = 0, int posY = 0, bool isActive = false);
//    ~pixelSelectionButton();

    bool isSelected () {return isSelected_;}

    int getRow () {return row_;}
    int getCol () {return col_;}

private slots:

    void on_theBotton_clicked();

private:

    int row_;
    int col_;
    bool isActive_;
    bool isSelected_;
};

#endif // PIXELSELECTIONBUTTON_H
