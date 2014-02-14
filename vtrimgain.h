#ifndef VTRIMGAIN_H
#define VTRIMGAIN_H

#include "scurveanalysis.h"
#include "TGraph.h"

class vTrimGain : public sCurveAnalysis
{
public:
    vTrimGain(sCurveAnalysis& anAnalysis);
    ~vTrimGain();

    std::map<QString, int> getOptimalValues () {return vTrimOptValues_;}

    TGraph* getGainPoint  (QString spc) {return gainPoint_ [spc];}
    TGraph* getGainPointX (QString spc) {return gainPointX_[spc];}
    TGraph* getGainPointY (QString spc) {return gainPointY_[spc];}

    int getTotGraphs () {return histogramSum2D.size();}

    int getMinThreasholdValue () {return minThreashold_;}

    int  findOptimumGain  (QString spc);
    void findOptimumGains (           );

    bool isGainAnalysisPossible () {return isGainAnalysisPossible_;}

private:
//    int findVCalStartBin (QString spc);

    std::map<QString, TGraph*> gainPoint_ ;
    std::map<QString, TGraph*> gainPointX_;
    std::map<QString, TGraph*> gainPointY_;

    bool isGainAnalysisPossible_;

    std::map<QString, int> vTrimOptValues_;

    int minThreashold_;
};

#endif // VTRIMGAIN_H
