#ifndef PIXELSINGLE_H
#define PIXELSINGLE_H

#include <QWidget>
#include "QPaintEvent"

namespace Ui {
class pixelSingle;
}

class pixelSingle : public QWidget
{
    Q_OBJECT
    
public:

    explicit pixelSingle(QWidget *parent = 0, int posX = 0, int posY = 0, bool isActive = false);
    ~pixelSingle();

    bool isSelected () {return isSelected_;}

    void setSelected (bool selected);

    int getRow () {return row_;}
    int getCol () {return col_;}
    
private slots:

    void on_theBotton_clicked();

signals:

    void hasBeenChecked();

private:
    Ui::pixelSingle *ui;
    void paint (QPaintEvent * ev);

    int row_;
    int col_;
    bool isActive_;
    bool isSelected_;
};

#endif // PIXELSINGLE_H
