#ifndef SCURVEANALYSIS_H
#define SCURVEANALYSIS_H

#include <map>
#include "filereader.h"

class sCurveAnalysis : public fileReader
{
public:
    sCurveAnalysis ();
    sCurveAnalysis (const sCurveAnalysis& anAnalysis);
    ~sCurveAnalysis();//TODO

    void setMaxRowCol (int maxR, int maxC);

    void initializeAnalysis (fileReader * reader);
    void loadSCurves(fileReader * reader);
    void fitCurves (fileReader *reader);

    float getFitMean  (QString spc, int row, int col);
    float getFitSigma (QString spc, int row, int col);
    bool  isThrAnalysisReady () {return isAnalysisReadyThr_;}
    bool  isCalAnalysisReady () {return isAnalysisReadyCal_;}

//    std::map<QString, std::map<std::pair<int, int>, TH1F* > > getSCurvesThr();
//    std::map<QString, std::map<std::pair<int, int>, TH1F* > > getSCurvesCal();

    TH1F* getThrFittedHisto (QString spc, int row, int col);
    TH1F* getCalFittedHisto (QString spc, int row, int col);

    TH2F* getMeanPixelDistributionVCal  (QString spc);
    TH2F* getMeanPixelDistributionVThr  (QString spc);
    TH2F* getSigmaPixelDistributionVCal (QString spc);
    TH2F* getSigmaPixelDistributionVThr (QString spc);

    TH1F* getThrMeanDistribution  (QString spc);
    TH1F* getThrSigmaDistribution (QString spc);
    TH1F* getCalMeanDistribution  (QString spc);
    TH1F* getCalSigmaDistribution (QString spc);

    void setFitMethod   (std::string method) {fitMethod_ += method;}
    void resetFitMethod (                  ) {fitMethod_ = "Q"    ;} //standard method has no terminal printing

    static double sCurveThr (double * x, double * par);
    static double sCurveCal (double * x, double * par);

    void clearOptVals ();

    std::map<QString, std::pair<int, int> >& getVCalOptimumFromMinimum() {return vCalOptimumFromMinimum;}
    std::map<QString, std::pair<int, int> >& getVThrOptimumFromMinimum() {return vThrOptimumFromMinimum;}
    std::map<QString, std::pair<int, int> >& getVCalOptimumFromFit    () {return vCalOptimumFromFit    ;}
    std::map<QString, std::pair<int, int> >& getVThrOptimumFromFit    () {return vThrOptimumFromFit    ;}

    void fitDistributions();

    void doVCalOptimumFromMinimum();
    void doVThrOptimumFromMinimum();
    void doVCalOptimumFromFit    ();
    void doVThrOptimumFromFit    ();


protected:

    int maxRows_;
    int maxCols_;

     std::map<QString, std::map<std::pair<int, int>, TH1F* > > theSCurvesVThr_ ;
     std::map<QString, std::map<std::pair<int, int>, TF1*  > > theFitFuncsVThr_;
     std::map<QString, std::map<std::pair<int, int>, TH1F* > > theSCurvesVCal_ ;
     std::map<QString, std::map<std::pair<int, int>, TF1*  > > theFitFuncsVCal_;

     bool isAnalysisInitialized_;
     bool isAnalysisReadyThr_   ;
     bool isAnalysisReadyCal_   ;
     bool isAnalysisDoneCal_    ;
     bool isAnalysisDoneThr_    ;
     bool isAnalysisFitDoneCal_ ;
     bool isAnalysisFitDoneThr_ ;

     std::map<QString, TF1*> theMeanFitFuncCal_;
     std::map<QString, TF1*> theMeanFitFuncThr_;

     std::map<QString, std::map<std::pair<int,int>, double> > theThreasholdBareVCal_;
     std::map<QString, std::map<std::pair<int,int>, double> > theThreasholdBareVThr_;

     std::string fitMethod_;

private:

     double findMeanRaw  (TH1F * histo, int scale); //TODO
     double findSigmaRaw (TH1F * histo, int scale); //TODO

      std::map<QString, TH1F*> theMeanDistributionCal_ ;
      std::map<QString, TH1F*> theSigmaDistributionCal_;
      std::map<QString, TH1F*> theMeanDistributionThr_ ;
      std::map<QString, TH1F*> theSigmaDistributionThr_;

      std::map<QString, TH2F*> theMeansVCal_ ;
      std::map<QString, TH2F*> theSigmasVCal_;
      std::map<QString, TH2F*> theMeansVThr_ ;
      std::map<QString, TH2F*> theSigmasVThr_;

      std::map<QString, std::pair<int, int> > vCalOptimumFromMinimum;
      std::map<QString, std::pair<int, int> > vThrOptimumFromMinimum;
      std::map<QString, std::pair<int, int> > vCalOptimumFromFit    ;
      std::map<QString, std::pair<int, int> > vThrOptimumFromFit    ;
 };

#endif // SCURVEANALYSIS_H
