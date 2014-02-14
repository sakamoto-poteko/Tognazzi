#include "vtrimgain.h"
#include <TF1.h>

vTrimGain::vTrimGain(sCurveAnalysis &anAnalysis) : sCurveAnalysis(anAnalysis)
{
    for (std::map<QString, std::map<std::string, TH2F*> >::iterator hIt = histogramSum2D.begin(); hIt != histogramSum2D.end(); ++ hIt)
    {
        gainPoint_ [hIt->first] = new TGraph();
        gainPointX_[hIt->first] = new TGraph();
        gainPointY_[hIt->first] = new TGraph();
    }

    if (!hasVariable("25. Vcal vs 11. Vtrim"))
    {
        std::cout << "No plot available for finding the VTrim gain: VTrim vs VCal needed!" << std::endl;
        isGainAnalysisPossible_ = false;
        return;
    }

    isGainAnalysisPossible_ = true;

    minThreashold_ = 0;

    std::cout << "Be sure that TrimBit is 15 now!\n";
}

vTrimGain::~vTrimGain()
{
    for (std::map<QString, std::map<std::string, TH2F*> >::iterator hIt = histogramSum2D.begin(); hIt != histogramSum2D.end(); ++ hIt)
    {
        delete gainPoint_ [hIt->first];
        delete gainPointX_[hIt->first];
        delete gainPointY_[hIt->first];
    }
}

int vTrimGain::findOptimumGain (QString spc)
{
    if (!isGainAnalysisPossible_)
    {
        std::cout << "Not Possible!\n";
        return -1;
    }

    if (!histogramSum2D[spc]["25. Vcal vs 11. Vtrim"])
    {
        std::cout << "Select an histogram of Vcal vs Vtrim!\n";
        return -1;
    }

    int VTrimOpt = -1;
    //Finding VTrim Min Value
    TH1D * tmp = histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->ProjectionY();
    int minYBin = 1;
    for (int i = 1; i < tmp->GetXaxis()->GetNbins(); ++i)
    {
        if (tmp->GetBinContent(i) > tmp->GetEntries()*0.001)
        {
            minYBin = i;
            std::cout << "Found a start point on y axis at " << minYBin << std::endl;
            break;
        }
    }
    delete tmp;

    //Finding VCal for VTrim Min Value
    float VCalStart = 0;
    std::cout << "Requiring a value > " << pixelInjection_[spc]["25. Vcal vs 11. Vtrim"]*pixelActive_[spc]["25. Vcal vs 11. Vtrim"]*0.9 << std::endl;
    for (int j = 1; j < histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->GetNbins(); ++j)
    {
        if (histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetBinContent(j, minYBin) >
                pixelInjection_[spc]["25. Vcal vs 11. Vtrim"]*pixelActive_[spc]["25. Vcal vs 11. Vtrim"]*0.9)
        {
            VCalStart = histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->GetBinCenter(j);
            break;
            std::cout << "Found a possible VCalStart at " << VCalStart << std::endl;
        }
        if (j == histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->GetNbins() -1)
        {
            VCalStart = histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->GetBinCenter(j);
            std::cout << "Found a possible VCalStart at " << VCalStart << " (last point on axis)" << std::endl;
        }
    }
    gainPointY_[spc]->SetPoint(0, histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetYaxis()->GetBinCenter(minYBin), VCalStart);

    float VCalEnd = -1;
    //adding 6 sigmas
    if (theMeanFitFuncCal_[spc])
    {
        VCalEnd = VCalStart - theMeanFitFuncCal_[spc]->GetParameter(1)*6.;
        std::cout << "VCal end point at " << VCalEnd << std::endl;
    }
    else
    {
        std::cout << "Threashold distribution not implemented for this chip, trying to use another one...\n";
        QRegExp reg("_VTrim");
        for (std::map<QString, TF1*>::iterator iu = theMeanFitFuncCal_.begin(); iu != theMeanFitFuncCal_.end(); ++iu)
        {
            if (reg.indexIn(iu->first) < 0 && theMeanFitFuncCal_.find(iu->first) != theMeanFitFuncCal_.end())
            {
                VCalEnd = VCalStart - theMeanFitFuncCal_[iu->first]->GetParameter(2)*6.;
                std::cout << "Found for " << iu->first.toStdString() << std::endl;
                std::cout << "VCal end point at " << VCalEnd << std::endl;
                break;
            }
        }
        if (VCalEnd == -1)
        {
            std::cout << "Couldn't find any valid threashold distribution, give up, sorry...\n";
            return -1;
        }
    }
    gainPointY_[spc]->SetPoint(0, histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetYaxis()->GetBinCenter(minYBin), VCalEnd);

    //finding VTrim for VCalEnd
    float VTrimOptFloat = -1;
    int k = 0;
    for (k = 0; k < histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetYaxis()->GetNbins() +1; ++k)
    {
        std::cout << "Iteration " << k << ": bin content = "
                 << histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetBinContent(histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->FindBin(VCalEnd), k) << std::endl;
        if (histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetBinContent(histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetXaxis()->FindBin(VCalEnd), k)
                > pixelInjection_[spc]["25. Vcal vs 11. Vtrim"]*pixelActive_[spc]["25. Vcal vs 11. Vtrim"]*0.9)
        {
            VTrimOptFloat = histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetYaxis()->GetBinCenter(k);
            VTrimOpt = (int)VTrimOptFloat;
            std::cout << "Found at " << VTrimOpt << std::endl;
            break;
        }
    }

    if (VTrimOptFloat == -1)
    {
        VTrimOptFloat = histogramSum2D[spc]["25. Vcal vs 11. Vtrim"]->GetYaxis()->GetBinCenter(k);
        VTrimOpt = (int)VTrimOptFloat;
        std::cout << "Found at " << VTrimOpt << " (end of iteration)" << std::endl;
    }

    gainPointX_[spc]->SetPoint(0, VTrimOptFloat, 0);
    gainPoint_ [spc]->SetPoint(0, VTrimOptFloat, VCalEnd);

    minThreashold_ = (int)VCalEnd;

    vTrimOptValues_[spc] = VTrimOpt;

    std::cout << spc.toStdString() << ": VTrim Optimal Value = " << VTrimOpt << std::endl;

    return VTrimOpt;
}

void vTrimGain::findOptimumGains ()
{
    for (std::map<QString, std::map<std::string, TH2F*> >::iterator hIt = histogramSum2D.begin(); hIt != histogramSum2D.end(); ++ hIt)
    {
       findOptimumGain(hIt->first);
    }
}
