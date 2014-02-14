#include "vtrimadjuster.h"

vTrimAdjuster::vTrimAdjuster ()
{
//    theVTrimVCalRelation_[""][std::pair<int,int>(0,0)] = 0;
//    theVTrimVThrRelation_[""][std::pair<int,int>(0,0)] = 0;
}

vTrimAdjuster::~vTrimAdjuster ()
{
/*    for (std::map<QString, std::map<std::pair<int, int>, TH1F* > >::iterator itT = theSCurvesVCal_.begin(); itT != theSCurvesVCal_.end(); ++itT)
    {
        if (!itT->first.isNull() && varTrimValue(itT->first) >= 0)
        {
            std::cout << "here\n";
            for (std::map<std::pair<int, int>, TH1F* >::iterator iyT = itT->second.begin(); iyT != itT->second.end(); ++ iyT)
            {
//                       if (theVTrimVCalRelation_.find(itT->first) != theVTrimVCalRelation_.end())
//                       {
//                           if (theVTrimVCalRelation_[itT->first].find(iyT->first) == theVTrimVCalRelation_[itT->first].end())
                        if (!theVTrimVCalRelation_[itT->first][iyT->first])
                        {
                            std::cout << "Deleting " << theVTrimVCalRelation_[itT->first][iyT->first]->GetName() << std::endl;
                            theVTrimVCalRelation_[itT->first][iyT->first]->Clear();
                            theVTrimVCalRelation_[iyT->first]->Delete();
                            delete theVTrimVCalRelation_[itT->first][iyT->first];
                        }
//                       }
             }
        }
    }
    */
}

vTrimAdjuster::vTrimAdjuster(fileReader *reader)
{
    ;
}

vTrimAdjuster::vTrimAdjuster(sCurveAnalysis& sCurves) : sCurveAnalysis(sCurves)
{
    areThrPlotsReady_ = false;
    areCalPlotsReady_ = false;

    if (isAnalysisReadyThr_)
    {
        int nPoints = 0;
        std::stringstream gName;
        std::stringstream gTitle;
        for (std::map<QString, std::map<std::pair<int, int>, TH1F* > >::iterator itT = theSCurvesVThr_.begin(); itT != theSCurvesVThr_.end(); ++itT)
        {
            if (!itT->first.isNull() && varTrimValue(itT->first) >= 0)
            {
                for (std::map<std::pair<int, int>, TH1F* >::iterator iyT = itT->second.begin(); iyT != itT->second.end(); ++ iyT)
                {
                    if (!theVTrimVThrRelation_[iyT->first])
                    {
                        gName.str(""); gTitle.str("");
                        gName << "fit_" << itT->first.toStdString() << "_r" << iyT->first.first << "_c" << iyT->first.second;
                        gTitle << "VTrim vs Threashold (Vcal) for chip " << itT->first.toStdString() << ", row " << iyT->first.first << ", col " << iyT->first.second;
                        theVTrimVThrRelation_[iyT->first] = new TGraphErrors();
                        theVTrimVThrRelation_[iyT->first]->GetXaxis()->SetTitle("VTrim");
                        theVTrimVThrRelation_[iyT->first]->GetYaxis()->SetTitle("Threashold");
                        std::cout << "Instantiating graph " << gName.str() << std::endl;
                    }
                    if (theSCurvesVThr_[itT->first][iyT->first]->GetEntries() > 0)
                    {
                        theVTrimVThrRelation_[iyT->first]->SetPoint     (nPoints, varTrimValue(itT->first), theFitFuncsVThr_[itT->first][iyT->first]->GetParameter(0));
                        theVTrimVThrRelation_[iyT->first]->SetPointError(nPoints,                      0.5, theFitFuncsVThr_[itT->first][iyT->first]->GetParError (0));
                     }
                     else
                     {
                        theVTrimVThrRelation_[iyT->first]->SetPoint     (nPoints, varTrimValue(itT->first),   1);
                        theVTrimVThrRelation_[iyT->first]->SetPointError(nPoints,                      0.5, 0.1);
                      }
                 }
                 ++nPoints;
            }
        }
        if (nPoints >= 2) areThrPlotsReady_ = true;
    }

    if (isAnalysisReadyCal_)
    {
        int nPoints = 0;
        std::stringstream gName;
        std::stringstream gTitle;
        for (std::map<QString, std::map<std::pair<int, int>, TH1F* > >::iterator itT = theSCurvesVCal_.begin(); itT != theSCurvesVCal_.end(); ++itT)
        {
            if (!itT->first.isNull() && varTrimValue(itT->first) >= 0)
            {
                for (std::map<std::pair<int, int>, TH1F* >::iterator iyT = itT->second.begin(); iyT != itT->second.end(); ++ iyT)
                {
                    if (!theVTrimVCalRelation_[iyT->first])
                    {
                        gName.str(""); gTitle.str("");
                        gName << "fit_" << itT->first.toStdString() << "_r" << iyT->first.first << "_c" << iyT->first.second;
                        gTitle << "VTrim vs Threashold (Vcal) for chip " << itT->first.toStdString() << ", row " << iyT->first.first << ", col " << iyT->first.second;
                        std::cout << "Parsing " << gName.str() << std::endl;
                        theVTrimVCalRelation_[iyT->first] = new TGraphErrors();
                        theVTrimVCalRelation_[iyT->first]->GetXaxis()->SetTitle("VTrim");
                        theVTrimVCalRelation_[iyT->first]->GetYaxis()->SetTitle("Threashold");
                    }
                    if (theSCurvesVCal_[itT->first][iyT->first]->GetEntries() > 0)
                    {
                        theVTrimVCalRelation_[iyT->first]->SetPoint     (nPoints, varTrimValue(itT->first), theFitFuncsVCal_[itT->first][iyT->first]->GetParameter(0));
                        theVTrimVCalRelation_[iyT->first]->SetPointError(nPoints,                      0.5, theFitFuncsVCal_[itT->first][iyT->first]->GetParError (0));
                     }
                     else
                     {
                        theVTrimVCalRelation_[iyT->first]->SetPoint     (nPoints, varTrimValue(itT->first),   1);
                        theVTrimVCalRelation_[iyT->first]->SetPointError(nPoints,                      0.5, 0.1);
                      }
                 }
                 ++nPoints;
            }
        }
        if (nPoints >= 2) areCalPlotsReady_ = true;
    }

    if (!areCalPlotsReady_ && !areThrPlotsReady_)
    {
        std::cout << "Impossible to adjust VTrim: not enough points of threashold vs VTrim." << std::endl;
    }
}

vTrimAdjuster::vTrimAdjuster(sCurveAnalysis& sCurves, int r, int c) : sCurveAnalysis(sCurves)
{
    areThrPlotsReady_ = false;
    areCalPlotsReady_ = false;

    if (isAnalysisReadyThr_)
    {
        int nPoints = 0;
        std::stringstream gName;
        std::stringstream gTitle;
        for (std::map<QString, std::map<std::pair<int, int>, TH1F* > >::iterator itT = theSCurvesVThr_.begin(); itT != theSCurvesVThr_.end(); ++itT)
        {
            if (!itT->first.isNull() && varTrimValue(itT->first) >= 0)
            {
                if (!theVTrimVCalRelation_[std::pair<int,int>(r,c)])
                {
                    gName.str(""); gTitle.str("");
                    gName << itT->first.toStdString() << "_r" << r << "_c" << c;
                    gTitle << "VTrim vs Threashold (VThr) for chip " << itT->first.toStdString() << ", row " << r << ", col " << c;
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)] = new TGraphErrors();
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->GetXaxis()->SetTitle("TrimBit");
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->GetYaxis()->SetTitle("Threashold");
                 }
                if (theSCurvesVThr_[itT->first][std::pair<int,int>(r,c)]->GetEntries() > 0)
                {
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->SetPoint     (nPoints, varTrimValue(itT->first), theFitFuncsVThr_[itT->first][std::pair<int,int>(r,c)]->GetParameter(0));
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->SetPointError(nPoints,                      0.5, theFitFuncsVThr_[itT->first][std::pair<int,int>(r,c)]->GetParError (0));
                }
                else
                {
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->SetPoint     (nPoints, varTrimValue(itT->first), -1);
                    theVTrimVThrRelation_[std::pair<int,int>(r,c)]->SetPointError(nPoints,                      0.5, 0.1);
                }
            }
            ++nPoints;
        }
        if (nPoints >= 2) areThrPlotsReady_ = true;
    }


    if (isAnalysisReadyCal_)
    {
        int nPoints = 0;
        std::stringstream gName;
        std::stringstream gTitle;
        for (std::map<QString, std::map<std::pair<int, int>, TH1F* > >::iterator itT = theSCurvesVCal_.begin(); itT != theSCurvesVCal_.end(); ++itT)
        {
            if (!itT->first.isNull() && varTrimValue(itT->first) >= 0)
            {
                if (!theVTrimVCalRelation_[std::pair<int,int>(r,c)])
                {
                    gName.str(""); gTitle.str("");
                    gName << "fit_" << itT->first.toStdString() << "_r" << r << "_c" << c;
                    gTitle << "VTrim vs Threashold (Vcal) for chip " << itT->first.toStdString() << ", row " << r << ", col " << c;
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)] = new TGraphErrors();
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->GetXaxis()->SetTitle("TrimBit");
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->GetYaxis()->SetTitle("Threashold");
                }
                if (theSCurvesVCal_[itT->first][std::pair<int,int>(r,c)]->GetEntries() > 0)
                {
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->SetPoint     (nPoints, varTrimValue(itT->first), theFitFuncsVCal_[itT->first][std::pair<int,int>(r,c)]->GetParameter(0));
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->SetPointError(nPoints,                      0.5, theFitFuncsVCal_[itT->first][std::pair<int,int>(r,c)]->GetParError (0));
                }
                else
                {
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->SetPoint     (nPoints, varTrimValue(itT->first),  -1);
                    theVTrimVCalRelation_[std::pair<int,int>(r,c)]->SetPointError(nPoints,                      0.5, 0.1);
                }
            }
            ++nPoints;
        }
        if (nPoints >= 2) areCalPlotsReady_ = true;
    }

    if (!areCalPlotsReady_ && !areThrPlotsReady_)
    {
        std::cout << "Impossible to adjust VTrim: not enough points of threashold vs VTrim." << std::endl;
    }
}

int vTrimAdjuster::getNofVTrimSteps (std::string isTorC)
{
    int n = 0;

    if (isTorC == "Thr")
    {
        for (std::map<std::pair<int, int>, TGraphErrors* >::iterator ie = theVTrimVThrRelation_.begin(); ie != theVTrimVThrRelation_.end(); ++ie)
        {
            ++n;
        }
        return n;
    }

    else if (isTorC == "Cal")
    {
        for (std::map<std::pair<int, int>, TGraphErrors* >::iterator ie = theVTrimVCalRelation_.begin(); ie != theVTrimVCalRelation_.end(); ++ie)
        {
            ++n;
        }
        return n;
    }

    else
    {
        std::cout << "Variable not recognized!" << std::endl;
        return 0;
    }
}

void vTrimAdjuster::fitRelation (int r, int c)
{
    std::stringstream ssf;
    if (areCalPlotsReady_)
    {
        if(theVTrimVCalRelationFit_[std::pair<int,int>(r,c)]) delete theVTrimVCalRelationFit_[std::pair<int,int>(r,c)];
        ssf.str("");
        ssf << "fitVtrimVCalSingle_r" << r << "_c" << c;
        theVTrimVCalRelationFit_[std::pair<int,int>(r,c)] = new TF1(ssf.str().c_str(), "[0] + [1]*x", theVTrimVCalRelation_[std::pair<int,int>(r,c)]->GetXaxis()->GetXmin(), theVTrimVCalRelation_[std::pair<int,int>(r,c)]->GetXaxis()->GetXmax());
        theVTrimVCalRelationFit_[std::pair<int,int>(r,c)]->SetParName(0, "Intercept");
        theVTrimVCalRelationFit_[std::pair<int,int>(r,c)]->SetParName(1, "Slope");
        theVTrimVCalRelationFit_[std::pair<int,int>(r,c)]->SetLineColor(kBlue);
        theVTrimVCalRelation_[std::pair<int,int>(r,c)]->Fit(theVTrimVCalRelationFit_[std::pair<int,int>(r,c)], fitMethod_.c_str());
    }

    if (areThrPlotsReady_)
    {
        if(theVTrimVThrRelationFit_[std::pair<int,int>(r,c)]) delete theVTrimVThrRelationFit_[std::pair<int,int>(r,c)];
        ssf.str("");
        ssf << "fitVtrimVThrSingle_r" << r << "_c" << c;
        theVTrimVThrRelationFit_[std::pair<int,int>(r,c)] = new TF1(ssf.str().c_str(), "[0] + [1]*x", theVTrimVThrRelation_[std::pair<int,int>(r,c)]->GetXaxis()->GetXmin(), theVTrimVThrRelation_[std::pair<int,int>(r,c)]->GetXaxis()->GetXmax());
        theVTrimVThrRelationFit_[std::pair<int,int>(r,c)]->SetParName(0, "Intercept");
        theVTrimVThrRelationFit_[std::pair<int,int>(r,c)]->SetParName(1, "Slope");
        theVTrimVThrRelationFit_[std::pair<int,int>(r,c)]->SetLineColor(kBlue);
        theVTrimVThrRelation_[std::pair<int,int>(r,c)]->Fit(theVTrimVThrRelationFit_[std::pair<int,int>(r,c)], fitMethod_.c_str());
    }
}

void vTrimAdjuster::fitRelations ()
{
    std::stringstream ssf;
    if (areCalPlotsReady_)
    {
        for (std::map<std::pair<int,int>, TGraphErrors*>::iterator it = theVTrimVCalRelation_.begin(); it != theVTrimVCalRelation_.end(); ++it)
        {
            if(theVTrimVCalRelationFit_[it->first]) delete theVTrimVCalRelationFit_[it->first];
            ssf.str("");
            ssf << "fitVtrimVCal_r" << it->first.first << "_c" << it->first.second;
            theVTrimVCalRelationFit_[it->first] = new TF1(ssf.str().c_str(), "[0] + [1]*x", theVTrimVCalRelation_[it->first]->GetXaxis()->GetXmin(), theVTrimVCalRelation_[it->first]->GetXaxis()->GetXmax());
            theVTrimVCalRelationFit_[it->first]->SetParName(0, "Intercept");
            theVTrimVCalRelationFit_[it->first]->SetParName(1, "Slope");
            theVTrimVCalRelationFit_[it->first]->SetLineColor(kBlue);
            theVTrimVCalRelation_[it->first]->Fit(theVTrimVCalRelationFit_[it->first], fitMethod_.c_str());
        }
    }

    if (areThrPlotsReady_)
    {
        for (std::map<std::pair<int,int>, TGraphErrors*>::iterator it = theVTrimVCalRelation_.begin(); it != theVTrimVCalRelation_.end(); ++it)
        {
            if(theVTrimVThrRelationFit_[it->first]) delete theVTrimVThrRelationFit_[it->first];
            ssf.str("");
            ssf << "fitVtrimVThr_r" << it->first.first << "_c" << it->first.second;
            theVTrimVThrRelationFit_[it->first] = new TF1(ssf.str().c_str(), "[0] + [1]*x", theVTrimVThrRelation_[it->first]->GetXaxis()->GetXmin(), theVTrimVThrRelation_[it->first]->GetXaxis()->GetXmax());
            theVTrimVThrRelationFit_[it->first]->SetParName(0, "Intercept");
            theVTrimVThrRelationFit_[it->first]->SetParName(1, "Slope");
            theVTrimVThrRelationFit_[it->first]->SetLineColor(kBlue);
            theVTrimVThrRelation_[it->first]->Fit(theVTrimVThrRelationFit_[it->first], fitMethod_.c_str());
        }
    }
}

int vTrimAdjuster::varTrimValue (QString var)
{
//     "Tel" + tel + "_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip + "VTrim" + Vtrim;
//    Telboh_Stat0_Plaq0_Chip0_VTrim7
    QRegExp varRec ("Tel\\w+_Stat\\d+_Plaq\\d+_Chip\\d+_VTrim(\\d+)");
    if (varRec.indexIn(var) >= 0)
        return varRec.cap(1).toInt();
    else
        return -1;
}
