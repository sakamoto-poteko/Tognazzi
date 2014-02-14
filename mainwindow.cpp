#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filereader.h"
#include "vtrimgain.h"
#include "vtrimset.h"

#include "QFileDialog"
#include "TStyle.h"
#include "QColor"

#include "TGraph.h"
#include "TGraph2D.h"

#include <fstream>

#define checkVar  if (!ui->variablesLW->currentItem())    { std::cout << "Select a Variable!" << std::endl; return;}
#define checkChip if (ui->spcCB->currentText().isNull()) { std::cout << "Select a Chip!!"    << std::endl; return;}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Tognazzi");

    theVariablesHandler_ = 0;
    reader_ = 0;
    sCurveAnalyzer_.setMaxRowCol(ui->MaxRSB->value(), ui->MaxCSB->value());
//    pixelSelector_ = new pixelSelection();
//    vTrimAdjuster_ = 0;

    ui->makePlotsPB->setEnabled(false);
    ui->optValFileSaveLE->setText("optimizationResults");
    ui->savedVTrimValuesLE->setText("optVTrimvalues");
    ui->saveTrimCorrectionsPB->setText("trimbitCorrection");
    resetProcedure();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete reader_;
    delete theVariablesHandler_;
//    delete sCurveAnalyzer_;
}

void MainWindow::on_pushButton_clicked()
{
    QString dataDir = variablesHandler::getEnvPath("TOGNAZZICALIBDIR");
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Text calibration file",dataDir,"Txt Files(*.txt)");
    if(fileNames.isEmpty())
        return;

    ui->makePlotsPB->setEnabled(true);
    resetProcedure();

    fileNames_ = fileNames;

    ui->fileNameLE->setText(fileNames.at(0));
    ui->spcCB->clear();
    ui->variablesLW->clear();
    ui->progressBar->setRange(0,100);
}

void MainWindow::on_makePlotsPB_clicked()
{
    if (reader_) delete reader_;
    reader_ = new fileReader(ui->uMessangerTE, ui->progressBar, fileNames_);

//    connect(reader_, SIGNAL(setProgress), this, SLOT(advanceProgressBar));

    ui->makePlotsPB->setEnabled(false);

    std::map<std::pair<int, int>, std::map<std::string, std::map< std::pair<int, int>, std::pair<int, int> > > > variablesMap = reader_->getListOfVariables(varNames_, ui->MaxRSB->value(), ui->MaxCSB->value());

//    sCurveAnalyzer_->setPixelInjection(reader_->getNumberOfInjections());

    if(theVariablesHandler_) delete theVariablesHandler_;
    theVariablesHandler_ = new variablesHandler(reader_, "cacca");

    if (reader_->hasVariable("12. VcThr") || reader_->hasVariable("25. Vcal"))
        ui->fitSCurvePB->setEnabled(true);

    for (std::map<QString, QStringList>::iterator im = varNames_.begin(); im !=  varNames_.end(); ++im)
        ui->spcCB->addItem(im->first);

    ui->smartSelectionPB->setEnabled(true);
    ui->fitADCPB->setEnabled(true);
//    if (reader_->hasVariable("25. Vcal vs 11. Vtrim"))
//        ui->vTrimOptPB->setEnabled(true);
}

void MainWindow::on_variablesLW_itemDoubleClicked(QListWidgetItem *item)
{
    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    else gStyle->SetOptStat(1);
    gStyle->SetPalette(ui->paletteSB->value());

    ui->canvasTQTW->GetCanvas()->Clear() ;
    ui->canvasTQTW->GetCanvas()->Modified() ;
    ui->canvasTQTW->GetCanvas()->Update();
    ui->canvasTQTW->GetCanvas()->cd();
    if (reader_->isVar2D(item->text()) == false)
    {
        if(reader_->getHisto1D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString())
           && ui->countsRB->isChecked())
            reader_->getHisto1D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString())->Draw();
        else if(reader_->getHistoAdc1D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString())
           && ui->adcRB->isChecked())
            reader_->getHistoAdc1D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString())->Draw();
    }
    else
    {
        if(reader_->getHisto2D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString()))
            reader_->getHisto2D(ui->spcCB->currentText(), ui->rowSB->value(), ui->colSB->value(), item->text().toStdString())->Draw(setDraw2DOpt().c_str());
    }
    ui->canvasTQTW->GetCanvas()->Modified();
    ui->canvasTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();

    ui->progressBar->setValue(100);

    return;
}

void MainWindow::on_smartSelectionPB_clicked()
{
    checkVar;
    checkChip;

    pixelSelector_ = new pixelSelection(this, reader_, ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString());
    pixelSelector_->setOneCheckable(false);
    pixelSelector_->show();
    connect(pixelSelector_, SIGNAL(drawPixels()), this, SLOT(drawPixelMultiple()));
}

void MainWindow::drawPixelMultiple()
{
    checkVar;

    gStyle->SetPalette(ui->paletteSB->value());

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    else gStyle->SetOptStat(1);

    int nPlots = pixelSelector_->getPixelsSelected().size();

    ui->canvasTQTW->GetCanvas()->Clear() ;
    ui->canvasTQTW->GetCanvas()->Modified() ;
    ui->canvasTQTW->GetCanvas()->Update();

    ui->canvasTQTW->GetCanvas()->Divide(optCanvasDivision(nPlots).first, optCanvasDivision(nPlots).second);

    for (int n = 1; n <= nPlots; ++n)
    {
        ui->canvasTQTW->GetCanvas()->cd(n);
        if (reader_->isVar2D(ui->variablesLW->currentItem()->text()) == false)
        {
            if(reader_->getHisto1D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString())
                && ui->countsRB->isChecked())
                reader_->getHisto1D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString())->Draw();
            else if(reader_->getHistoAdc1D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString())
                && ui->adcRB->isChecked())
                reader_->getHistoAdc1D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString())->Draw();
        }
        else
        {
            if(reader_->getHisto2D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString()))
                reader_->getHisto2D(ui->spcCB->currentText(), pixelSelector_->getPixelsSelected().at(n-1).first, pixelSelector_->getPixelsSelected().at(n-1).second, ui->variablesLW->currentItem()->text().toStdString())->Draw(setDraw2DOpt().c_str());
        }
    }

    ui->canvasTQTW->GetCanvas()->Modified();
    ui->canvasTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
    return;
}

void MainWindow::on_spcCB_currentIndexChanged(const QString &arg1)
{
    ui->variablesLW->clear();
    ui->variablesLW->addItems(varNames_.find(arg1)->second);
}

std::string MainWindow::setDraw2DOpt()
{
    std::string opt;
    if      (ui->colzRB ->isChecked()) opt = "COLZ" ;
    else if (ui->surf1RB->isChecked()) opt = "surf1";
    else if (ui->surf2RB->isChecked()) opt = "surf2";
    else if (ui->legoRB ->isChecked()) opt = "LEGO" ;
    else if (ui->lego2RB->isChecked()) opt = "LEGO2";
    else opt = "";

    return opt;
}

void MainWindow::resetProcedure()
{
    ui->fileNameLE->clear();
    ui->plotDistributionsB->setEnabled(false);
//    ui->plotAnalysisPB->setEnabled(false);
    ui->fitSCurvePB->setEnabled(false);
    ui->plotDistributionsB->setEnabled(false);
    ui->gaussFitPB->setEnabled(false);
    ui->OptValPB->setEnabled(false);
    ui->saveResultPB->setEnabled(false);
    ui->savFilePB->setEnabled(false);
    ui->optValFileSaveLE->setEnabled(false);
    ui->fitCB->setEnabled(false);
    ui->calVTrimPB->setEnabled(false);
    ui->vTrimOptPB->setEnabled(false);
    ui->saveVTrimValuesPB->setEnabled(false);
    ui->setTrimBitPB->setEnabled(false);
    ui->smartSelectionPB->setEnabled(false);
    ui->Thr2DPlotPB->setEnabled(false);
    ui->fitADCPB->setEnabled(false);
}

void MainWindow::on_MaxRSB_valueChanged(int arg1)
{
    ui->rowSB->setMaximum(arg1);
    ui->rowSB_A->setMaximum(arg1);
}

void MainWindow::on_MaxCSB_valueChanged(int arg1)
{
    ui->colSB->setMaximum(arg1);
    ui->colSB_A->setMaximum(arg1);
}

void MainWindow::on_savFilePB_clicked()
{
    reader_->saveFile();
}

void MainWindow::on_fitADCPB_clicked()
{
    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);

    reader_->fitHistosAdc();
}
void MainWindow::on_sumPlotsPB_clicked()
{
    checkVar;

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    else gStyle->SetOptStat(1);
    gStyle->SetPalette(ui->paletteSB->value());

    ui->canvasAverageTQTW->GetCanvas()->Clear() ;
    ui->canvasAverageTQTW->GetCanvas()->Modified() ;
    ui->canvasAverageTQTW->GetCanvas()->Update();
    ui->canvasAverageTQTW->GetCanvas()->cd();
    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    else gStyle->SetOptStat(1);
    if (reader_->isVar2D(ui->variablesLW->currentItem()->text()) == false)
    {
        if(reader_->getHistoSum1D(ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString()))
        {
            reader_->getHistoSum1D(ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString())->Draw();
        }
    }
    else
    {
        if(reader_->getHistoSum2D(ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString()))
            reader_->getHistoSum2D(ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString())->Draw(setDraw2DOpt().c_str());
    }
    ui->canvasAverageTQTW->GetCanvas()->Modified();
    ui->canvasAverageTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
    return;

}

void MainWindow::on_fitSCurvePB_clicked()
{
//    sCurveAnalyzer_ = new sCurveAnalysis(ui->MaxRSB->value(), ui->MaxCSB->value());
    sCurveAnalyzer_.initializeAnalysis(reader_);
    sCurveAnalyzer_.loadSCurves(reader_);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    if (ui->likelihoodCB->isChecked()) sCurveAnalyzer_.setFitMethod("L");
    sCurveAnalyzer_.fitCurves(reader_);
    sCurveAnalyzer_.resetFitMethod();
    ui->plotDistributionsB->setEnabled(true);
    ui->Thr2DPlotPB->setEnabled(true);
    ui->gaussFitPB->setEnabled(true);
    ui->OptValPB->setEnabled(true);
    if (sCurveAnalyzer_.isVtrimScanPossible())
    {
        ui->calVTrimPB->setEnabled(true);
        ui->saveTrimCorrectionsPB->setEnabled(true);
    }
    ui->fineTunePB->setEnabled(false);
}

/*void MainWindow::on_plotAnalysisPB_clicked()   //It crashes... boh. Not necesary because you can go back to the first tab and you'll see the same
{
    if (!sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
    {
        std::cout << "No analysis ready!" << std::endl;
        return;
    }

    if (sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->Divide(1,2);
        ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
        sCurveAnalyzer_.getCalFittedHisto(ui->variablesLW->currentItem()->text(), ui->rowSB_A->value(), ui->colSB_A->value())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
        sCurveAnalyzer_.getThrFittedHisto(ui->variablesLW->currentItem()->text(), ui->rowSB_A->value(), ui->colSB_A->value())->Draw();
    }

    if (sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->cd();
        sCurveAnalyzer_.getCalFittedHisto(ui->variablesLW->currentItem()->text(), ui->rowSB_A->value(), ui->colSB_A->value())->Draw();
    }

    if (!sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasTQTW->GetCanvas()->Clear();
        gPad->Update();
        ui->canvasAnalysisTQTW->GetCanvas()->cd();
        sCurveAnalyzer_.getThrFittedHisto(ui->variablesLW->currentItem()->text(), ui->rowSB_A->value(), ui->colSB_A->value())->Draw();
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
    return;

}
*/

void MainWindow::on_plotDistributionsB_clicked()
{
//    checkVar;

    gStyle->SetPalette(ui->paletteSB->value());

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    if (!sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
    {
        std::cout << "No analysis ready!" << std::endl;
        return;
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
//    ui->canvasAnalysisTQTW->GetCanvas()->cd();
//    gPad->Update();

    if (sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->Divide(2,2);
        ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
        if (sCurveAnalyzer_.getCalMeanDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getCalMeanDistribution(ui->spcCB->currentText())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
        if (sCurveAnalyzer_.getCalSigmaDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getCalSigmaDistribution(ui->spcCB->currentText())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(3);
        if (sCurveAnalyzer_.getThrMeanDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getThrMeanDistribution(ui->spcCB->currentText())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(4);
        if (sCurveAnalyzer_.getThrSigmaDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getThrSigmaDistribution(ui->spcCB->currentText())->Draw();
    }

    if (sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->Divide(2,1);
        ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
        if (sCurveAnalyzer_.getCalMeanDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getCalMeanDistribution(ui->spcCB->currentText())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
        if (sCurveAnalyzer_.getCalSigmaDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getCalSigmaDistribution(ui->spcCB->currentText())->Draw();
    }

    if (!sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->Divide(2,1);
        ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
        if (sCurveAnalyzer_.getThrMeanDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getThrMeanDistribution(ui->spcCB->currentText())->Draw();
        ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
        if (sCurveAnalyzer_.getThrSigmaDistribution(ui->spcCB->currentText()))
            sCurveAnalyzer_.getThrSigmaDistribution(ui->spcCB->currentText())->Draw();
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
    return;
}

void MainWindow::on_Thr2DPlotPB_clicked()
{
//    checkVar;
    checkChip;

    gStyle->SetPalette(ui->paletteSB->value());

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    if (!sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
    {
        std::cout << "No analysis ready!" << std::endl;
        return;
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
//    ui->canvasAnalysisTQTW->GetCanvas()->cd();
//    gPad->Update();

    if (ui->thr2DRB->isChecked())
    {
        if (sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->Divide(1,1);
            ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
            if (sCurveAnalyzer_.getMeanPixelDistributionVCal(ui->spcCB->currentText()))
                sCurveAnalyzer_.getMeanPixelDistributionVCal(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
            ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
            if (sCurveAnalyzer_.getMeanPixelDistributionVThr(ui->spcCB->currentText()))
                sCurveAnalyzer_.getMeanPixelDistributionVThr(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }

        if (sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->cd();
            if (sCurveAnalyzer_.getMeanPixelDistributionVCal(ui->spcCB->currentText()))
                sCurveAnalyzer_.getMeanPixelDistributionVCal(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }

        if (!sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->cd();
            if (sCurveAnalyzer_.getMeanPixelDistributionVThr(ui->spcCB->currentText()))
                sCurveAnalyzer_.getMeanPixelDistributionVThr(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }
    }

    else if (ui->sigma2DRB->isChecked())
    {
        if (sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->Divide(1,1);
            ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
            if (sCurveAnalyzer_.getSigmaPixelDistributionVCal(ui->spcCB->currentText()))
                sCurveAnalyzer_.getSigmaPixelDistributionVCal(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
            ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
            if (sCurveAnalyzer_.getSigmaPixelDistributionVThr(ui->spcCB->currentText()))
                sCurveAnalyzer_.getSigmaPixelDistributionVThr(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }

        if (sCurveAnalyzer_.isCalAnalysisReady() && !sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->cd();
            if (sCurveAnalyzer_.getSigmaPixelDistributionVCal(ui->spcCB->currentText()))
                sCurveAnalyzer_.getSigmaPixelDistributionVCal(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }

        if (!sCurveAnalyzer_.isCalAnalysisReady() && sCurveAnalyzer_.isThrAnalysisReady())
        {
            ui->canvasAnalysisTQTW->GetCanvas()->cd();
            if (sCurveAnalyzer_.getSigmaPixelDistributionVThr(ui->spcCB->currentText()))
                sCurveAnalyzer_.getSigmaPixelDistributionVThr(ui->spcCB->currentText())->Draw(setDraw2DOpt().c_str());
        }
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
    return;
}

//void MainWindow::on_unZoomPB_clicked()
//{
//}

void MainWindow::on_gaussFitPB_clicked()
{
    if (ui->likelihoodCB->isChecked()) sCurveAnalyzer_.setFitMethod("L");
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    sCurveAnalyzer_.fitDistributions();
    sCurveAnalyzer_.resetFitMethod();
    ui->fitCB->setEnabled(true);
    ui->saveTrimCorrectionsPB->setEnabled(true);
}

void MainWindow::on_OptValPB_clicked()
{
    if (!ui->minCB->isChecked() && !ui->fitCB->isChecked())
    {
        std::cout << "Set at least one method of finding the optimal value!" << std::endl;
        return;
    }

    sCurveAnalyzer_.clearOptVals();

    if (ui->likelihoodCB->isChecked()) sCurveAnalyzer_.setFitMethod("L");
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);

    if (ui->minCB->isChecked())
    {
        if (sCurveAnalyzer_.isCalAnalysisReady())
        {
            sCurveAnalyzer_.doVCalOptimumFromMinimum();
            ui->optValFileSaveLE->setEnabled(true);
            ui->saveResultPB->setEnabled(true);
        }
        if (sCurveAnalyzer_.isThrAnalysisReady())
        {
            sCurveAnalyzer_.doVThrOptimumFromMinimum();
            ui->optValFileSaveLE->setEnabled(true);
            ui->saveResultPB->setEnabled(true);
        }
        if(!sCurveAnalyzer_.isCalAnalysisReady() && ! sCurveAnalyzer_.isThrAnalysisReady())
        {
            std::cout << "Do one analysis at least!" << std::endl;
        }
    }

    if (ui->fitCB->isChecked())
    {
//        std::cout << "Fit analysis not implemented yet!" << std::endl;
//        return;
        if (sCurveAnalyzer_.isCalAnalysisReady())
        {
            sCurveAnalyzer_.doVCalOptimumFromFit();
            ui->optValFileSaveLE->setEnabled(true);
            ui->saveResultPB->setEnabled(true);
        }
        if (sCurveAnalyzer_.isThrAnalysisReady())
        {
            sCurveAnalyzer_.doVThrOptimumFromFit();
            ui->optValFileSaveLE->setEnabled(true);
            ui->saveResultPB->setEnabled(true);
        }
        if(!sCurveAnalyzer_.isCalAnalysisReady() && ! sCurveAnalyzer_.isThrAnalysisReady())
        {
            std::cout << "Do one analysis at least!" << std::endl;
        }
    }

    sCurveAnalyzer_.resetFitMethod();
}

void MainWindow::on_saveResultPB_clicked()
{
    QString dataDir = variablesHandler::getEnvPath("TOGNAZZIOUTPUTDIR");
    std::ofstream outFile;
    outFile.open((dataDir + ui->optValFileSaveLE->text() + (QString)".txt").toStdString().c_str());

    outFile << "################## This is the result of the optimization analysis of VThr/VCal obtained with Tognazzi ##################\n";

    if (sCurveAnalyzer_.getVCalOptimumFromMinimum().empty() == false && sCurveAnalyzer_.isCalAnalysisReady())
    {
        outFile << "----------- From Histogram Minimum -----------\n";
        outFile << "Row: Tel           Stat |Plaq  |Chip  -- VCalMin | VCalMax\n";
        for (std::map<QString, std::pair<int,int> >::iterator itC = sCurveAnalyzer_.getVCalOptimumFromMinimum().begin(); itC != sCurveAnalyzer_.getVCalOptimumFromMinimum().end(); ++itC)
        {
            outFile << "     " << itC->first.toStdString() << " -- " << itC->second.first << " | " << itC->second.second << "\n";
        }
        outFile << "----------- From Histogram Fit     -----------\n";
        outFile << "Row: Tel           Stat |Plaq  |Chip  -- VThrMin | VThrMax\n";
        for (std::map<QString, std::pair<int,int> >::iterator itC = sCurveAnalyzer_.getVCalOptimumFromFit().begin(); itC != sCurveAnalyzer_.getVCalOptimumFromFit().end(); ++itC)
        {
            outFile << "     " << itC->first.toStdString() << " -- " << itC->second.first << "     | " << itC->second.second << "\n";
        }
    }

    if (sCurveAnalyzer_.getVThrOptimumFromMinimum().empty() == false && sCurveAnalyzer_.isThrAnalysisReady())
    {
        outFile << "----------- From Histogram Minimum -----------\n";
        outFile << "Row: Tel           Stat |Plaq  |Chip  -- VThrMin | VThrMax\n";
        for (std::map<QString, std::pair<int,int> >::iterator itT = sCurveAnalyzer_.getVThrOptimumFromMinimum().begin(); itT != sCurveAnalyzer_.getVThrOptimumFromMinimum().end(); ++itT)
        {
            outFile << "     " << itT->first.toStdString() << " -- " << itT->second.first << "     | " << itT->second.second << "\n";
//            std::cout<< "     " << itT->first.toStdString() << " -- " << itT->second.first << "     | " << itT->second.second << "\n";
        }
        outFile << "----------- From Histogram Fit     -----------\n";
        outFile << "Row: Tel           Stat |Plaq  |Chip  -- VThrMin | VThrMax\n";
        for (std::map<QString, std::pair<int,int> >::iterator itT = sCurveAnalyzer_.getVThrOptimumFromFit().begin(); itT != sCurveAnalyzer_.getVThrOptimumFromFit().end(); ++itT)
        {
            outFile << "     " << itT->first.toStdString() << " -- " << itT->second.first << "     | " << itT->second.second << "\n";
//            std::cout<< "     " << itT->first.toStdString() << " -- " << itT->second.first << "     | " << itT->second.second << "\n";
        }
    }

    outFile << "##################                                     END OF FILE                                    ##################\n";

    outFile.close();
}

void MainWindow::on_calVTrimPB_clicked()
{
//    vTrimAdjuster vTrimAdjuster_(sCurveAnalyzer_);
//    if (!vTrimAdjuster_.areCalPlotsReady() && !vTrimAdjuster_.areThrPlotsReady())
//        return;
    ui->plotVTrimPB->setEnabled(true);
}

void MainWindow::on_plotVTrimPB_clicked()
{
    vTrimAdjuster vTrimAdjuster_(sCurveAnalyzer_, ui->rowSB_A->value(), ui->colSB_A->value());
    if (!vTrimAdjuster_.areCalPlotsReady() && !vTrimAdjuster_.areThrPlotsReady())
    {
        std::cout << "Something wrong!" << std::endl;
        return;
    }

    gStyle->SetPalette(ui->paletteSB->value());

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);

    vTrimAdjuster_.fitRelation(ui->rowSB_A->value(), ui->colSB_A->value());

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();

    if (vTrimAdjuster_.areCalPlotsReady() && vTrimAdjuster_.areThrPlotsReady())
    {
//        int nPlots = vTrimAdjuster_.getNofVTrimSteps("Thr");
        ui->canvasAnalysisTQTW->GetCanvas()->Divide(1,2);
        if (!vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value()) &&
             !vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value()))
        {
            ui->canvasAnalysisTQTW->GetCanvas()->cd(1);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->Draw("AP");
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerColor(kMagenta);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerStyle(8);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerSize(1.5);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetTitle("Threashold as function of Vtrim (VCal computed)");
            ui->canvasAnalysisTQTW->GetCanvas()->cd(2);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerColor(kMagenta);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerStyle(8);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerSize(1.5);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetTitle("Threashold as function of Vtrim (VThr computed)");
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->Draw("AP");
        }
        else
        {
            std::cout << "Graph not existing!\n";
            return;
        }
    }

    else if (vTrimAdjuster_.areCalPlotsReady() && !vTrimAdjuster_.areThrPlotsReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->cd();
        if (vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value()))
        {
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerColor(kMagenta);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerStyle(8);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerSize(1.5);
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetTitle("Threashold as function of Vtrim (VCal computed)");
            vTrimAdjuster_.getVTrimVCalRelation(ui->rowSB_A->value(), ui->colSB_A->value())->Draw("AP");
        }
        else
        {
            std::cout << "Graph not existing!\n";
            return;
        }
    }

    else if (!vTrimAdjuster_.areCalPlotsReady() && vTrimAdjuster_.areThrPlotsReady())
    {
        ui->canvasAnalysisTQTW->GetCanvas()->cd();
        if (vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value()))
        {
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerColor(kMagenta);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerStyle(8);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetMarkerSize(1.5);
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->SetTitle("Threashold as function of Vtrim (VThr computed)");
            vTrimAdjuster_.getVTrimVThrRelation(ui->rowSB_A->value(), ui->colSB_A->value())->Draw("AP");
        }
        else
        {
            std::cout << "Graph not existing!\n";
            return;
        }
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();

    ui->vTrimOptPB->setEnabled(true);
    ui->saveVTrimValuesPB->setEnabled(true);

    return;
}

void MainWindow::on_smartSelection_trimPB_clicked()
{
    checkVar;
    checkChip;

    pixelSelector_ = new pixelSelection(this, reader_, ui->spcCB->currentText(), ui->variablesLW->currentItem()->text().toStdString());
    pixelSelector_->show();
//    connect(pixelSelector_, SIGNAL(drawPixels()), this, SLOT(drawPixelMultiple()));
}

void MainWindow::on_vTrimOptPB_clicked()
{
    checkChip;

//    sCurveAnalyzer_.loadSCurves(reader_);
    vTrimGain gain(sCurveAnalyzer_);
    if (!gain.isGainAnalysisPossible())
    {
        std::cout << "Ooops!\n";
        return;
    }

    int optValue = gain.findOptimumGain(ui->spcCB->currentText());

    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    gStyle->SetPalette(ui->paletteSB->value());

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();

    ui->canvasAnalysisTQTW->GetCanvas()->cd();
    if (sCurveAnalyzer_.getHistoSum2D(ui->spcCB->currentText(), "25. Vcal vs 11. Vtrim"))
    {
        sCurveAnalyzer_.getHistoSum2D(ui->spcCB->currentText(), "25. Vcal vs 11. Vtrim")->Draw("COLZ");
        gain.getGainPoint (ui->spcCB->currentText())->SetMarkerColor(kMagenta);
        gain.getGainPoint (ui->spcCB->currentText())->SetMarkerStyle(8);
        gain.getGainPoint (ui->spcCB->currentText())->SetMarkerSize(1.5);
//        gain.getGainPoint (ui->spcCB->currentText())->Draw("AP, same");
        gain.getGainPointX(ui->spcCB->currentText())->SetMarkerColor(kBlue);
        gain.getGainPointX(ui->spcCB->currentText())->SetMarkerStyle(8);
        gain.getGainPointX(ui->spcCB->currentText())->SetMarkerSize(1);
//        gain.getGainPointX(ui->spcCB->currentText())->Draw("P, same");
        gain.getGainPointY(ui->spcCB->currentText())->SetMarkerColor(kBlue);
        gain.getGainPointY(ui->spcCB->currentText())->SetMarkerStyle(8);
        gain.getGainPointY(ui->spcCB->currentText())->SetMarkerSize(1);
//        gain.getGainPointY(ui->spcCB->currentText())->Draw("P, same");
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();

    ui->optValueLE->setText((QString)optValue);

    ui->fineTunePB->setEnabled(true);
    ui->setTrimBitPB->setEnabled(true);
}

void MainWindow::on_fineTunePB_clicked()
{
    checkVar;
    checkChip;

/*    vTrimGain gain(sCurveAnalyzer_);
    if (!gain.isGainAnalysisPossible())
    {
        std::cout << "Ooops!\n";
        return;
    }

    int optValue = gain.findOptimumGain(ui->spcCB->currentText());
    theTuner_ = new fineTuner(this, optValue, gain.getMinThreasholdValue());
*/
    theTuner_ = new fineTuner(this, 100, 100);
    theTuner_->show();
    connect(theTuner_, SIGNAL(superimposeCanvas()), this, SLOT(superImposeFineTuning()));
}

void MainWindow::superImposeFineTuning ()
{
    if (ui->displayStatCB->isChecked() == false) gStyle->SetOptStat(0);
    if (ui->displayFitCB->isChecked()) gStyle->SetOptFit(111111);
    gStyle->SetPalette(ui->paletteSB->value());

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();

    ui->canvasAnalysisTQTW->GetCanvas()->cd();
//    sCurveAnalyzer_.getHistoSum2D(ui->spcCB->currentText(), "25. Vcal vs 11. Vtrim")->Draw("COLZ");
    TH2F tmp = *reader_->getHistoSum2D(ui->spcCB->currentText(), "12. VcThr vs 26. CalDel");
/*    TGraph2D tmp;
    tmp.SetPoint(0, theTuner_->getCurrentX(), theTuner_->getCurrentY(), 1000);
//    tmp.SetPoint(0, 100, 100);
    tmp.SetMarkerColor(kMagenta);
    tmp.SetMarkerStyle(8);
    tmp.SetMarkerSize(1.5);
    tmp.Draw("P, same");
    */

    tmp.SetBinContent(theTuner_->getCurrentX(), theTuner_->getCurrentY(),
                       reader_->getHistoSum2D(ui->spcCB->currentText(), "12. VcThr vs 26. CalDel")->GetBinContent(tmp.FindBin(theTuner_->getCurrentX(), theTuner_->getCurrentY()), tmp.FindBin(theTuner_->getCurrentX(), theTuner_->getCurrentY())) + 10);
    tmp.Draw("COLZ");

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();
}

void MainWindow::on_saveVTrimValuesPB_clicked()
{
    checkChip;

    vTrimGain gain(sCurveAnalyzer_);
    if (!gain.isGainAnalysisPossible())
    {
        std::cout << "Ooops!\n";
        return;
    }

    gain.findOptimumGains();

    ui->canvasAnalysisTQTW->GetCanvas()->Clear() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Modified() ;
    ui->canvasAnalysisTQTW->GetCanvas()->Update();

    ui->canvasAnalysisTQTW->GetCanvas()->Divide(optCanvasDivision(gain.getTotGraphs()).first,
                                                optCanvasDivision(gain.getTotGraphs()).second);

    int n = 1;
    for(std::map<QString, int>::iterator it = gain.getOptimalValues().begin(); it != gain.getOptimalValues().end(); ++it)
    {
        ui->canvasAnalysisTQTW->GetCanvas()->cd(n);
        sCurveAnalyzer_.getHistoSum2D(it->first, "25. Vcal vs 11. Vtrim")->Draw();
        gain.getGainPoint (it->first)->SetMarkerColor(kMagenta);
        gain.getGainPoint (it->first)->SetMarkerStyle(8);
        gain.getGainPoint (it->first)->SetMarkerSize(1.5);
        gain.getGainPoint (it->first)->Draw("AP, same");
        gain.getGainPointX(it->first)->SetMarkerColor(kBlue);
        gain.getGainPointX(it->first)->SetMarkerStyle(8);
        gain.getGainPointX(it->first)->SetMarkerSize(1);
        gain.getGainPointX(it->first)->Draw("P, same");
        gain.getGainPointY(it->first)->SetMarkerColor(kBlue);
        gain.getGainPointY(it->first)->SetMarkerStyle(8);
        gain.getGainPointY(it->first)->SetMarkerSize(1);
        gain.getGainPointY(it->first)->Draw("P, same");
        ++n;
    }

    ui->canvasAnalysisTQTW->GetCanvas()->Modified();
    ui->canvasAnalysisTQTW->GetCanvas()->Update();
    gPad->Modified();
    gPad->Update();

    QString dataDir = variablesHandler::getEnvPath("TOGNAZZIOUTPUTDIR");
    std::ofstream outFile;
    outFile.open((dataDir + ui->savedVTrimValuesLE->text() + (QString)".txt").toStdString().c_str());

    outFile << "################## This is the result of the optimization analysis of VTrim for each chip obtained with Tognazzi ##################\n";
    outFile << " Chip " << " --- VTrim " << std::endl;
    for (std::map<QString, int>::iterator it = gain.getOptimalValues().begin(); it != gain.getOptimalValues().end(); ++it)
    {
        outFile << it->first.toStdString() << " --- " << it->second << std::endl;
    }
    outFile << "##################                                     END OF FILE                                    ##################\n";

    outFile.close();
}

void MainWindow::on_setTrimBitPB_clicked()
{
    vTrimSet trimSet (sCurveAnalyzer_);

    QString dataDir = variablesHandler::getEnvPath("TOGNAZZIOUTPUTDIR");
    std::ofstream outFile;
    outFile.open((dataDir + ui->savedVTrimValuesLE->text() + (QString)".txt").toStdString().c_str());

    outFile << "################## This is the result of the optimization analysis of TrimBit for each chip obtained with Tognazzi ##################\n";
    outFile << " Chip:    ";
    for (std::map<QString, std::map<std::pair<int,int>, int> >::iterator ih = trimSet.getTrimCorrections().begin(); ih != trimSet.getTrimCorrections().end(); ++ih)
    {
        outFile << ih->first.toStdString() << std::endl;
        for (std::map<std::pair<int,int>, int>::iterator io = ih->second.begin(); io != ih->second.end(); ++io)
        {
//            if (trimSet.isPixelActive(ih->first, "25. Vcal vs 11. Vtrim", ))
            outFile << "row " << io->first.first << " col " << io->first.second << " trimbit " << io->second;
        }
    }
    outFile << "##################                                     END OF FILE                                    ##################\n";

    outFile.close();
}

std::pair<int,int> MainWindow::optCanvasDivision (int totCanvas)
{
    if (totCanvas == 2)
        return std::pair<int,int>(1,2);

    else if (totCanvas > 2 && totCanvas <=4)
        return std::pair<int,int>(2,2);

    else if (totCanvas > 4 && totCanvas <=6)
        return std::pair<int,int>(2,3);

    else if (totCanvas > 6 && totCanvas <=9)
        return std::pair<int,int>(3,3);

    else if (totCanvas > 9 && totCanvas <=12)
        return std::pair<int,int>(4,3);

    else if (totCanvas > 12 && totCanvas <=16)
        return std::pair<int,int>(4,4);

    else if (totCanvas > 16 && totCanvas <=25)
        return std::pair<int,int>(5,5);

    else if (totCanvas > 25 && totCanvas <=36)
        return std::pair<int,int>(6,6);

    else if (totCanvas > 36 && totCanvas <=49)
        return std::pair<int,int>(7,7);

    else return std::pair<int,int>(1,1); //more than 49 is unlikely
}

void MainWindow::advanceProgressBar (int value)
{
    ui->progressBar->setValue(value);
}
