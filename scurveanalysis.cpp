#include "scurveanalysis.h"

#include "TMath.h"
#include "TF1.h"

sCurveAnalysis::sCurveAnalysis() : fileReader()
{
//    maxRows_ = maxRows;
//    maxCols_ = maxCols;
    isAnalysisInitialized_ = false;
    isAnalysisReadyThr_    = false;
    isAnalysisReadyCal_    = false;
    isAnalysisDoneCal_     = false;
    isAnalysisDoneThr_     = false;
    isAnalysisFitDoneCal_  = false;
    isAnalysisFitDoneThr_  = false;
    fitMethod_ = "Q";
}

sCurveAnalysis::~sCurveAnalysis()
{
    ;
}

void sCurveAnalysis::setMaxRowCol(int maxR, int maxC)
{
    maxRows_ = maxR;
    maxCols_ = maxC;
}

sCurveAnalysis::sCurveAnalysis (const sCurveAnalysis& anAnalysis) : fileReader()
{
    maxRows_ = anAnalysis.maxRows_;
    maxCols_ = anAnalysis.maxCols_;

    histogramSum1D  = anAnalysis.histogramSum1D ;
    histogramSum2D  = anAnalysis.histogramSum2D ;
    pixelInjection_ = anAnalysis.pixelInjection_;
    pixelActive_    = anAnalysis.pixelActive_   ;
    isPixelActive_  = anAnalysis.isPixelActive_ ;
    isVtrimScan_    = anAnalysis.isVtrimScan_   ;

    isAnalysisInitialized_ = anAnalysis.isAnalysisInitialized_;
    isAnalysisReadyThr_    = anAnalysis.isAnalysisReadyThr_;
    isAnalysisReadyCal_    = anAnalysis.isAnalysisReadyCal_;
    isAnalysisDoneThr_    = anAnalysis.isAnalysisDoneThr_;
    isAnalysisDoneCal_    = anAnalysis.isAnalysisDoneCal_;
    isAnalysisFitDoneThr_    = anAnalysis.isAnalysisFitDoneThr_;
    isAnalysisFitDoneCal_    = anAnalysis.isAnalysisFitDoneCal_;

    if (isAnalysisReadyThr_)
    {
        theSCurvesVThr_  = anAnalysis.theSCurvesVThr_;
        theFitFuncsVThr_ = anAnalysis.theFitFuncsVThr_;
    }
    if (isAnalysisDoneThr_)
    {
        theMeanDistributionThr_  = anAnalysis.theMeanDistributionThr_ ;
        theSigmaDistributionThr_ = anAnalysis.theSigmaDistributionThr_;
        vThrOptimumFromMinimum   = anAnalysis.vThrOptimumFromMinimum  ;
    }
    if (isAnalysisFitDoneThr_)
    {
        theMeanFitFuncThr_     = anAnalysis.theMeanFitFuncThr_    ;
        vThrOptimumFromFit     = anAnalysis.vThrOptimumFromFit    ;
        theThreasholdBareVThr_ = anAnalysis.theThreasholdBareVThr_;
    }
    if (isAnalysisReadyCal_)
    {
        theSCurvesVCal_  = anAnalysis.theSCurvesVCal_;
        theFitFuncsVCal_ = anAnalysis.theFitFuncsVCal_;
    }
    if (isAnalysisDoneCal_)
    {
        theMeanDistributionCal_  = anAnalysis.theMeanDistributionCal_;
        theSigmaDistributionCal_ = anAnalysis.theSigmaDistributionCal_;
        vCalOptimumFromMinimum = anAnalysis.vCalOptimumFromMinimum;
    }
    if (isAnalysisFitDoneCal_)
    {
        theMeanFitFuncCal_     = anAnalysis.theMeanFitFuncCal_    ;
        vCalOptimumFromFit     = anAnalysis.vCalOptimumFromFit    ;
        theThreasholdBareVCal_ = anAnalysis.theThreasholdBareVCal_;
    }

     maxRows_ = anAnalysis.maxRows_;
     maxCols_ = anAnalysis.maxCols_;

     fitMethod_ = anAnalysis.fitMethod_;
}

double sCurveAnalysis::sCurveThr (double *x, double *par)
{
    if (x[0] - par[0] >= 0) return par[2]*(TMath::Erf((-(x[0] - par[0])/par[1]))/2 + 0.5);
    else return par[2]*(-TMath::Erf(((x[0] - par[0])/par[1]))/2 + 0.5);
}

double sCurveAnalysis::sCurveCal (double *x, double *par)
{
    if (x[0] - par[0] >= 0) return par[2]*(TMath::Erf(((x[0] - par[0])/par[1]))/2 + 0.5);
    else return par[2]*(-TMath::Erf((-(x[0] - par[0])/par[1]))/2 + 0.5);
}

double sCurveAnalysis::findMeanRaw (TH1F * histo, int scale)
{
    int i = 0;
    for (i  = 1; i < histo->GetXaxis()->GetNbins(); ++i)
    {
        if (TMath::Abs(histo->GetBinContent(i)-(double)scale*0.5) <= (double)scale*0.3)
        {
            return histo->GetBinCenter(i);
        }
    }
    if (i >=  histo->GetXaxis()->GetNbins()) return (histo->GetXaxis()->GetXmin() + histo->GetXaxis()->GetXmax())/2;
    else return 120.;
}

double sCurveAnalysis::findSigmaRaw (TH1F * histo, int scale)
{
    int mean = -1;

    for (int i  = 1; i < histo->GetXaxis()->GetNbins(); ++i)
    {
        if (TMath::Abs(histo->GetBinContent(i)-(double)scale*0.5) <= (double)scale*0.3) mean = i;
    }
    if (mean = -1) return 1;

    int j;
//    bool arrived = false;
    for (j = 0; j < 100; ++j)
    {
        if (histo->GetBinContent(mean + j) > (double)scale*0.8 && histo->GetBinContent(mean - j) < (double)scale*0.5)
            return TMath::Abs(histo->GetBinCenter(mean + j) - histo->GetBinCenter(mean));
        if (histo->GetBinContent(mean - j) < (double)scale*0.3 && histo->GetBinContent(mean + j) > (double)scale*0.5)
            return TMath::Abs(histo->GetBinCenter(mean - j) - histo->GetBinCenter(mean));
        if (histo->GetBinContent(mean - j) > (double)scale*0.8 && histo->GetBinContent(mean + j) < (double)scale*0.5)
            return TMath::Abs(histo->GetBinCenter(mean - j) - histo->GetBinCenter(mean));
        if (histo->GetBinContent(mean + j) < (double)scale*0.3 && histo->GetBinContent(mean - j) > (double)scale*0.5)
            return TMath::Abs(histo->GetBinCenter(mean + j) - histo->GetBinCenter(mean));
    }

    if (j >= 100) return 1;
    else return 1;
}

/*
std::map<QString, std::map<std::pair<int, int>, TH1F* > > sCurveAnalysis::getSCurvesThr()
{
    if (isAnalysisReadyThr_)
        return theSCurvesVThr_;
    else
    {
        std::cout << "VThr analysis not done!" << std::endl;
        return 0;
    }
}

std::map<QString, std::map<std::pair<int, int>, TH1F* > > sCurveAnalysis::getSCurvesCal()
{
    if (isAnalysisReadyCal_)
        return theSCurvesVCal_;
    else
    {
        std::cout << "VCal analysis not done!" << std::endl;
        return 0;
    }
}
*/

TH1F* sCurveAnalysis::getThrFittedHisto (QString spc, int row, int col)
{
    if (!isAnalysisReadyThr_)
    {
        std::cout << "Threahold analysis not ready!" << std::endl;
        return 0;
    }
    return theSCurvesVThr_[spc][std::pair<int,int>(row,col)];
}

TH1F* sCurveAnalysis::getCalFittedHisto (QString spc, int row, int col)
{
    if (!isAnalysisReadyCal_)
    {
        std::cout << "Calibration analysis not ready!" << std::endl;
        return 0;
    }
    return theSCurvesVCal_[spc][std::pair<int,int>(row,col)];
}

void sCurveAnalysis::initializeAnalysis (fileReader *reader)
{
    std::string hName, hTitle;
    std::stringstream ssName, ssTitle;

    if (reader->hasVariable("12. VcThr"))
    {
        QStringList chipListT = reader->getListOfChips("12. VcThr");
        for (int k = 0; k < chipListT.size(); ++k)
        {
            std::cout << "Found VThr for " << chipListT.at(k).toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                theSCurvesVThr_[chipListT.at(k)][std::pair<int,int>(i,j)] = 0;
                }
            ssName.str(""); ssTitle.str("");
            ssName << "chipThrValues_VThr_" << chipListT.at(k).toStdString();
            ssTitle << "Threshold values from VThr for each chip, " << chipListT.at(k).toStdString();
            theMeansVThr_[chipListT.at(k)] = new TH2F(ssName.str().c_str(), ssTitle.str().c_str(), maxCols_, 0 , maxCols_, maxRows_, 0, maxRows_);
            theMeansVThr_[chipListT.at(k)]->GetXaxis()->SetTitle("Column");
            theMeansVThr_[chipListT.at(k)]->GetYaxis()->SetTitle("Row");
            ssName.str(""); ssTitle.str("");
            ssName << "chipSigValues_VThr_" << chipListT.at(k).toStdString();
            ssTitle << "Threshold sigma values from VThr for each chip, " << chipListT.at(k).toStdString();
            theSigmasVThr_[chipListT.at(k)] = new TH2F(ssName.str().c_str(), ssTitle.str().c_str(), maxCols_, 0 , maxCols_, maxRows_, 0, maxRows_);
            theSigmasVThr_[chipListT.at(k)]->GetXaxis()->SetTitle("Column");
            theSigmasVThr_[chipListT.at(k)]->GetYaxis()->SetTitle("Row");
            hName = "theMeanDistributionThr_" + chipListT.at(k).toStdString();
            hTitle = "Distribution of threaholds after VThr scan, " + chipListT.at(k).toStdString();
            theMeanDistributionThr_ [chipListT.at(k)] = new TH1F (hName.c_str(), hTitle.c_str(), 255, 0., 255.);
            theMeanDistributionThr_ [chipListT.at(k)]->GetXaxis()->SetTitle("Threashold");
            theMeanDistributionThr_ [chipListT.at(k)]->GetYaxis()->SetTitle("counts");
            hName = "theSigmaDistributionThr_" + chipListT.at(k).toStdString();
            hTitle = "Distribution of threaholds uncertainties after VThr scan, " + chipListT.at(k).toStdString();
            theSigmaDistributionThr_[chipListT.at(k)] = new TH1F (hName.c_str(), hTitle.c_str(), 255*2, 0., 25.5);
            theSigmaDistributionThr_ [chipListT.at(k)]->GetXaxis()->SetTitle("Threashold Sigma");
            theSigmaDistributionThr_ [chipListT.at(k)]->GetYaxis()->SetTitle("counts");
        }
        isAnalysisInitialized_ = true;
    }
    if (reader->hasVariable("25. Vcal"))
    {
        QStringList chipListC = reader->getListOfChips("25. Vcal");
        for (int k = 0; k < chipListC.size(); ++k)
        {
            std::cout << "Found VCal for " << chipListC.at(k).toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                    theSCurvesVCal_[chipListC.at(k)][std::pair<int,int>(i,j)] = 0;
                }
            ssName.str(""); ssTitle.str("");
            ssName << "chipThrValues_VCal_" << chipListC.at(k).toStdString();
            ssTitle << "Threshold values from VCal for each chip, " << chipListC.at(k).toStdString();
            theMeansVCal_[chipListC.at(k)] = new TH2F(ssName.str().c_str(), ssTitle.str().c_str(), maxCols_, 0 , maxCols_, maxRows_, 0, maxRows_);
            theMeansVCal_[chipListC.at(k)]->GetXaxis()->SetTitle("Column");
            theMeansVCal_[chipListC.at(k)]->GetYaxis()->SetTitle("Row");
            ssName.str(""); ssTitle.str("");
            ssName << "chipSigValues_VCal_" << chipListC.at(k).toStdString();
            ssTitle << "Threshold sigma values from VCal for each chip, " << chipListC.at(k).toStdString();
            theSigmasVCal_[chipListC.at(k)] = new TH2F(ssName.str().c_str(), ssTitle.str().c_str(), maxCols_, 0 , maxCols_, maxRows_, 0, maxRows_);
            theSigmasVCal_[chipListC.at(k)]->GetXaxis()->SetTitle("Column");
            theSigmasVCal_[chipListC.at(k)]->GetYaxis()->SetTitle("Row");
            hName = "theMeanDistributionCal_" + chipListC.at(k).toStdString();
            hTitle = "Distribution of threaholds after VCal scan, " + chipListC.at(k).toStdString();
            theMeanDistributionCal_ [chipListC.at(k)] = new TH1F (hName.c_str(), hTitle.c_str(), 255, 0., 255.);
            theMeanDistributionCal_ [chipListC.at(k)]->GetXaxis()->SetTitle("Threashold");
            theMeanDistributionCal_ [chipListC.at(k)]->GetYaxis()->SetTitle("counts");
            hName = "theSigmaDistributionCal_" + chipListC.at(k).toStdString();
            hTitle = "Distribution of threaholds uncertainties after VCal scan, " + chipListC.at(k).toStdString();
            theSigmaDistributionCal_[chipListC.at(k)] = new TH1F (hName.c_str(), hTitle.c_str(), 255*2, 0., 25.5);
            theSigmaDistributionCal_ [chipListC.at(k)]->GetXaxis()->SetTitle("Threashold Sigma");
            theSigmaDistributionCal_ [chipListC.at(k)]->GetYaxis()->SetTitle("counts");
        }
        isAnalysisInitialized_ = true;
     }
}

TH1F * sCurveAnalysis::getCalMeanDistribution (QString spc)
{
    if (theMeanDistributionCal_.find(spc) == theMeanDistributionCal_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theMeanDistributionCal_[spc];
}

TH1F * sCurveAnalysis::getCalSigmaDistribution (QString spc)
{
    if (theSigmaDistributionCal_.find(spc) == theSigmaDistributionCal_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theSigmaDistributionCal_[spc];
}

TH1F * sCurveAnalysis::getThrMeanDistribution (QString spc)
{
    if (theMeanDistributionThr_.find(spc) == theMeanDistributionThr_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theMeanDistributionThr_[spc];
}

TH1F * sCurveAnalysis::getThrSigmaDistribution (QString spc)
{
    if (theSigmaDistributionThr_.find(spc) == theSigmaDistributionThr_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theSigmaDistributionThr_[spc];
}

TH2F * sCurveAnalysis::getMeanPixelDistributionVCal (QString spc)
{
    if (theMeansVCal_.find(spc) == theMeansVCal_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theMeansVCal_[spc];
}

TH2F * sCurveAnalysis::getSigmaPixelDistributionVCal (QString spc)
{
    if (theSigmasVCal_.find(spc) == theSigmasVCal_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theSigmasVCal_[spc];
}

TH2F * sCurveAnalysis::getMeanPixelDistributionVThr (QString spc)
{
    if (theMeansVThr_.find(spc) == theMeansVThr_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theMeansVThr_[spc];
}

TH2F * sCurveAnalysis::getSigmaPixelDistributionVThr (QString spc)
{
    if (theSigmasVThr_.find(spc) == theSigmasVThr_.end())
    {
        std::cout << "No threashold distribution for " << spc.toStdString() << std::endl;
        return 0;
    }
    if (!isAnalysisInitialized_)
    {
        std::cout << "Fit curves first!" << std::endl;
        return 0;
    }
    else
        return theSigmasVThr_[spc];
}

void sCurveAnalysis::clearOptVals ()
{
    if (!vCalOptimumFromMinimum.empty()) vCalOptimumFromMinimum.clear();
    if (!vCalOptimumFromFit    .empty()) vCalOptimumFromFit    .clear();
    if (!vThrOptimumFromMinimum.empty()) vThrOptimumFromMinimum.clear();
    if (!vThrOptimumFromFit    .empty()) vThrOptimumFromFit    .clear();
}

void sCurveAnalysis::fitDistributions ()
{
    std::string fTitle;

    if (!isAnalysisReadyThr_ && !isAnalysisReadyCal_)
    {
        std::cout << "Do ate least one plot before!\n";
        return;
    }

    if(isAnalysisReadyCal_)
    {
        for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionCal_.begin(); ikT != theMeanDistributionCal_.end(); ++ikT)
        {
            if (theMeanFitFuncCal_[ikT->first]) delete theMeanFitFuncCal_[ikT->first];
//            doVCalOptimumFromMinimum();
            fTitle = "fitMeanThreasholdDistributionVCal_" + ikT->first.toStdString();
            theMeanFitFuncCal_[ikT->first] = new TF1 (fTitle.c_str(), "gaus", 0, 255);
            theMeanFitFuncCal_[ikT->first]->SetLineColor(kBlue);
            std::cout << "Fitting " << ikT->first.toStdString() << std::endl;
            ikT->second->Fit(theMeanFitFuncCal_[ikT->first], fitMethod_.c_str());
        }
        isAnalysisFitDoneCal_ = true;
    }

    if (isAnalysisReadyThr_)
    {
        for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionThr_.begin(); ikT != theMeanDistributionThr_.end(); ++ikT)
        {
            if (theMeanFitFuncThr_[ikT->first]) delete theMeanFitFuncThr_[ikT->first];
//            doVThrOptimumFromMinimum();
            fTitle = "fitMeanThreasholdDistributionVThr_" + ikT->first.toStdString();
            theMeanFitFuncThr_[ikT->first] = new TF1 (fTitle.c_str(), "gaus", 0, 255);
            theMeanFitFuncThr_[ikT->first]->SetLineColor(kBlue);
            ikT->second->Fit(theMeanFitFuncThr_[ikT->first], fitMethod_.c_str());
        }
        isAnalysisFitDoneThr_ = true;
    }
}

void sCurveAnalysis::doVThrOptimumFromMinimum ()
{
    for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionThr_.begin(); ikT != theMeanDistributionThr_.end(); ++ikT)
    {
        int valueMin = 1000;
        int valueMax = -1;
        for (int i = 1; i < ikT->second->GetXaxis()->GetNbins(); ++i)
        {
            if (ikT->second->GetBinContent(i) > 0 && ikT->second->GetBinCenter(i) < valueMin) valueMin = ikT->second->GetBinCenter(i);
            if (ikT->second->GetBinContent(i) > 0 && ikT->second->GetBinCenter(i) > valueMax) valueMax = ikT->second->GetBinCenter(i);
        }
        vThrOptimumFromMinimum[ikT->first] = std::pair<int,int>(valueMin, valueMax);
//        std::cout << ikT->first.toStdString() << " - " << valueMin << ", " << valueMax << std::endl;
    }
    return;
}

void sCurveAnalysis::doVCalOptimumFromMinimum ()
{
    for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionCal_.begin(); ikT != theMeanDistributionCal_.end(); ++ikT)
    {
        int valueMin = 1000;
        int valueMax = -1;
        for (int i = 1; i < ikT->second->GetXaxis()->GetNbins(); ++i)
        {
            if (ikT->second->GetBinContent(i) > 0 && ikT->second->GetBinCenter(i) < valueMin) valueMin = ikT->second->GetBinCenter(i);
            if (ikT->second->GetBinContent(i) > 0 && ikT->second->GetBinCenter(i) > valueMax) valueMax = ikT->second->GetBinCenter(i);
        }
        vCalOptimumFromMinimum[ikT->first] = std::pair<int,int>(valueMin, valueMax);
    }
    return;
}

void sCurveAnalysis::doVCalOptimumFromFit ()
{
//    std::string fTitle;
    for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionCal_.begin(); ikT != theMeanDistributionCal_.end(); ++ikT)
    {
//        fTitle = "fitMeanThreasholdDistributionVCal_" + ikT->first.toStdString();
//        theMeanFitFuncCal_[ikT->first] = new TF1 (fTitle.c_str(), "gaus(0)", 0, 255);
//        ikT->second->Fit(theMeanFitFuncCal_[ikT->first], fitMethod_.c_str());
        vCalOptimumFromFit[ikT->first] = std::pair<int,int>
                ((int)(theMeanFitFuncCal_[ikT->first]->GetParameter(1) - 3*theMeanFitFuncCal_[ikT->first]->GetParameter(2)),
                 (int)(theMeanFitFuncCal_[ikT->first]->GetParameter(1) + 3*theMeanFitFuncCal_[ikT->first]->GetParameter(2)));
    }
}

void sCurveAnalysis::doVThrOptimumFromFit ()
{
//    std::string fTitle;
    for (std::map<QString, TH1F*>::iterator ikT = theMeanDistributionThr_.begin(); ikT != theMeanDistributionThr_.end(); ++ikT)
    {
//        fTitle = "fitMeanThreasholdDistributionVThr_" + ikT->first.toStdString();
//        theMeanFitFuncThr_[ikT->first] = new TF1 (fTitle.c_str(), "gaus(0)", 0, 255);
//        ikT->second->Fit(theMeanFitFuncThr_[ikT->first], fitMethod_.c_str());
        vThrOptimumFromFit[ikT->first] = std::pair<int,int>
                ((int)(theMeanFitFuncThr_[ikT->first]->GetParameter(1) - 3*theMeanFitFuncThr_[ikT->first]->GetParameter(2)),
                 (int)(theMeanFitFuncThr_[ikT->first]->GetParameter(1) + 3*theMeanFitFuncThr_[ikT->first]->GetParameter(2)));
    }
}

void sCurveAnalysis::loadSCurves (fileReader *reader)
{
    histogramSum1D  = reader->getHistosSum1D     ();
    histogramSum2D  = reader->getHistosSum2D     ();
    pixelActive_    = reader->getPixelActive     ();
    pixelInjection_ = reader->getPixelInjection  ();
    isVtrimScan_    = reader->isVtrimScanPossible();

    if (reader->hasVariable("12. VcThr"))
    {
        QStringList chipList = reader->getListOfChips("12. VcThr");
        for (int k = 0; k < chipList.size(); ++k)
        {
            std::cout << "Loading VThr for " << chipList.at(k).toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                    theSCurvesVThr_[chipList.at(k)][std::pair<int,int>(i,j)] = reader->getHisto1D(chipList.at(k), i, j, "12. VcThr");
                }
        }
        isAnalysisReadyThr_ = true;
    }

    if (reader->hasVariable("25. Vcal"))
    {
        QStringList chipList = reader->getListOfChips("25. Vcal");
        for (int k = 0; k < chipList.size(); ++k)
        {
            std::cout << "Loading VCal for " << chipList.at(k).toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                    theSCurvesVCal_[chipList.at(k)][std::pair<int,int>(i,j)] = reader->getHisto1D(chipList.at(k), i, j, "25. Vcal");
                }
        }
        isAnalysisReadyCal_ = true;
    }

    if(reader->hasVariable("12. VcThr") == false && reader->hasVariable("25. Vcal") == false)
    {
        std::cout << "Neither VThreashold nor VCal scan loaded!" << std::endl;
        isAnalysisReadyCal_ = false;
        isAnalysisReadyThr_ = false;
    }
}

void sCurveAnalysis::fitCurves (fileReader *reader)
{
    if (!isAnalysisReadyThr_ && !isAnalysisReadyCal_)
    {
        std::cout << "Neither VThreashold nor VCal scan loaded, Jesus!" << std::endl;
        return;
    }

    if (isAnalysisReadyThr_)
    {
        std::stringstream fName;
        float min, max;
        std::cout << "here!\n";
        for (std::map<QString, std::map<std::pair<int, int>, TH1F*> >::iterator it = theSCurvesVThr_.begin(); it != theSCurvesVThr_.end(); ++it)
        {
            std::cout << "Found VThr for " << it->first.toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                    if (theSCurvesVThr_[it->first][std::pair<int,int>(i,j)]->GetEntries() > 0)
                    {
                        if (theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]) delete theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)];
                        fName.str("");
                        fName << "fitSCurve_VThr_" << it->first.toStdString() << "_r" << i << "c" << j;
                        min = theSCurvesVThr_[it->first][std::pair<int,int>(i,j)]->GetXaxis()->GetXmin();
                        max = theSCurvesVThr_[it->first][std::pair<int,int>(i,j)]->GetXaxis()->GetXmax();
                        theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)] = new TF1(fName.str().c_str(), sCurveThr, min, max, 3);
                        theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParName(0, "Mean"        );
                        theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParName(1, "Sigma"       );
                        theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParName(2, "Scale Factor");
                        if (theSCurvesVThr_[it->first][std::pair<int,int>(i,j)]->GetEntries() > 0)
                        {
                            pixelActive_[it->first]["12. VcThr"] += 1;
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(0, findMeanRaw (theSCurvesVThr_[it->first][std::pair<int,int>(i,j)], reader->getNumberOfInjections(it->first, "12. VcThr")));
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(1, findSigmaRaw(theSCurvesVThr_[it->first][std::pair<int,int>(i,j)], reader->getNumberOfInjections(it->first, "12. VcThr")));
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(2, reader->getNumberOfInjections(it->first, "12. VcThr"));
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetLineColor(kRed);
                            std::cout << "Fitting  VThr SCurve for chip " << it->first.toStdString() << ", pixel: row " << i << " col " << j << std::endl;
                            theSCurvesVThr_[it->first][std::pair<int,int>(i,j)]->Fit(theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)], fitMethod_.c_str());
                            theMeanDistributionThr_ [it->first]->Fill(theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->GetParameter(0));
                            theSigmaDistributionThr_[it->first]->Fill(theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->GetParameter(1));
                            theThreasholdBareVThr_[it->first][std::pair<int,int>(i,j)] = theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->GetParameter(0);
                            theMeansVThr_ [it->first]->Fill(j,i, theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->GetParameter(0));
                            theSigmasVThr_[it->first]->Fill(j,i, theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->GetParameter(1));
                        }
                        else
                        {
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)] = new TF1(fName.str().c_str(), sCurveCal, min, max, 3);
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(0, 0);
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(1, 1);
                            theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]->SetParameter(2, 1);
                        }
                    }
                }
        }
        isAnalysisDoneThr_ = true;
    }

    if (isAnalysisReadyCal_)
    {
        std::stringstream fName;
        float min, max;
        for (std::map<QString, std::map<std::pair<int, int>, TH1F*> >::iterator it = theSCurvesVCal_.begin(); it != theSCurvesVCal_.end(); ++it)
        {
            std::cout << "Found VCal for " << it->first.toStdString() << std::endl;
            for (int i = 0; i < maxRows_; ++i)
                for (int j = 0; j < maxCols_; ++j)
                {
                    if (theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)]) delete theFitFuncsVThr_[it->first][std::pair<int,int>(i,j)];
                    fName.str("");
                    fName << "fitSCurve_VCal_" << it->first.toStdString() << "_r" << i << "c" << j;
                    min = theSCurvesVCal_[it->first][std::pair<int,int>(i,j)]->GetXaxis()->GetXmin();
                    max = theSCurvesVCal_[it->first][std::pair<int,int>(i,j)]->GetXaxis()->GetXmax();
                    theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)] = new TF1(fName.str().c_str(), sCurveCal, min, max, 3);
                    theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParName(0, "Mean"        );
                    theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParName(1, "Sigma"       );
                    theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParName(2, "Scale Factor");
                   if (theSCurvesVCal_[it->first][std::pair<int,int>(i,j)]->GetEntries() > 0)
                    {
                        pixelActive_[it->first]["25. Vcal"] += 1;
                        theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(0, findMeanRaw (theSCurvesVCal_[it->first][std::pair<int,int>(i,j)], reader->getNumberOfInjections(it->first, "25. Vcal")));
                        theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(1, findSigmaRaw(theSCurvesVCal_[it->first][std::pair<int,int>(i,j)], reader->getNumberOfInjections(it->first, "25. Vcal")));
                        theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(2, reader->getNumberOfInjections(it->first, "25. Vcal"));
                        theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetLineColor(kRed);
                        std::cout << "Fitting  VCal SCurve for chip " << it->first.toStdString() << ", pixel: row " << i << " col " << j << std::endl;
                        theSCurvesVCal_[it->first][std::pair<int,int>(i,j)]->Fit(theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)], fitMethod_.c_str());
                        theMeanDistributionCal_ [it->first]->Fill(theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->GetParameter(0));
                        theSigmaDistributionCal_[it->first]->Fill(theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->GetParameter(1));
                        theThreasholdBareVCal_[it->first][std::pair<int,int>(i,j)] = theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->GetParameter(0);
                        theMeansVCal_ [it->first]->Fill(j,i, theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->GetParameter(0));
                        theSigmasVCal_[it->first]->Fill(j,i, theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->GetParameter(1));
                    }
                   else
                   {
                       theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)] = new TF1(fName.str().c_str(), sCurveCal, min, max, 3);
                       theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(0, 0);
                       theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(1, 1);
                       theFitFuncsVCal_[it->first][std::pair<int,int>(i,j)]->SetParameter(2, 1);
                   }
                }
        }
        isAnalysisDoneCal_ = true;
    }
}
