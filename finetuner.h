#ifndef FINETUNER_H
#define FINETUNER_H

#include <QMainWindow>

namespace Ui {
class fineTuner;
}

class fineTuner : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit fineTuner(QWidget *parent = 0, int initX = 0, int initY = 0);
    ~fineTuner();
    
    int getCurrentX () {return currentX_;}
    int getCurrentY () {return currentY_;}

signals:

    void superimposeCanvas();

private slots:
    void on_xPosD_valueChanged(int value);

    void on_yPosD_valueChanged(int value);

private:
    Ui::fineTuner *ui;

    int currentX_;
    int currentY_;
};

#endif // FINETUNER_H
