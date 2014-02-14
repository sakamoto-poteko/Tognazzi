#ifndef VTRIMADJUSTER_H
#define VTRIMADJUSTER_H

#include "scurveanalysis.h"
#include "TGraphErrors.h"
#include "TF1.h"

class vTrimAdjuster : public sCurveAnalysis
{
public:
    vTrimAdjuster();
    vTrimAdjuster(fileReader * reader);
    vTrimAdjuster(sCurveAnalysis &sCurves);
    vTrimAdjuster(sCurveAnalysis &sCurves, int r, int c);
    ~vTrimAdjuster();

    bool areThrPlotsReady () {return areThrPlotsReady_;}
    bool areCalPlotsReady () {return areCalPlotsReady_;}

    TGraphErrors * getVTrimVCalRelation (int row, int col) { return theVTrimVCalRelation_[std::pair<int,int>(row,col)];}
    TGraphErrors * getVTrimVThrRelation (int row, int col) { return theVTrimVThrRelation_[std::pair<int,int>(row,col)];}

    int getNofVTrimSteps(std::string isTorC); //BAD

    void fitRelation  (int r, int c);
    void fitRelations (            );

private:

    int varTrimValue (QString var);

    std::map<std::pair<int, int>, TGraphErrors* > theVTrimVCalRelation_;
    std::map<std::pair<int, int>, TGraphErrors* > theVTrimVThrRelation_;

    std::map<std::pair<int, int>, TF1* > theVTrimVCalRelationFit_;
    std::map<std::pair<int, int>, TF1* > theVTrimVThrRelationFit_;

    bool areThrPlotsReady_;
    bool areCalPlotsReady_;
};

#endif // VTRIMADJUSTER_H
