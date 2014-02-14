#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "variableshandler.h"
#include "QListWidgetItem"
#include "scurveanalysis.h"
#include "vtrimadjuster.h"
#include "pixelselection.h"
#include "qmdiarea.h"
#include "qmdisubwindow.h"
#include "QTimer"
#include "finetuner.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_variablesLW_itemDoubleClicked(QListWidgetItem *item);

    void on_spcCB_currentIndexChanged(const QString &arg1);

    void on_MaxRSB_valueChanged(int arg1);

    void on_MaxCSB_valueChanged(int arg1);

    void on_savFilePB_clicked();

    void on_makePlotsPB_clicked();

    void on_sumPlotsPB_clicked();

    void on_fitSCurvePB_clicked();

//    void on_plotAnalysisPB_clicked();

    void on_plotDistributionsB_clicked();

//    void on_unZoomPB_clicked(); to be implemented!

    void on_OptValPB_clicked();

    void on_saveResultPB_clicked();

    void on_gaussFitPB_clicked();

    void on_calVTrimPB_clicked();

    void on_plotVTrimPB_clicked();

    void on_vTrimOptPB_clicked();

    void on_saveVTrimValuesPB_clicked();

    void on_setTrimBitPB_clicked();

    void on_smartSelectionPB_clicked();

    void drawPixelMultiple();

    void on_Thr2DPlotPB_clicked();

    void on_fineTunePB_clicked();

    void superImposeFineTuning();

    void on_fitADCPB_clicked();

    void on_smartSelection_trimPB_clicked();

protected slots:
    void advanceProgressBar (int value);

private:

    pixelSelection * pixelSelector_;

    fineTuner * theTuner_;

    std::string setDraw2DOpt();

    void resetProcedure();

    std::pair<int,int> optCanvasDivision(int totCanvas);

    QStringList fileNames_;

    std::map<QString, QStringList> varNames_;

    variablesHandler* theVariablesHandler_;

    fileReader * reader_;

    sCurveAnalysis sCurveAnalyzer_;

//    vTrimAdjuster vTrimAdjuster_;

    QTimer * theTimer_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
