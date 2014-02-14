#include "vtrimset.h"
#include "TF1.h"

vTrimSet::vTrimSet(sCurveAnalysis& anAnalysis) : vTrimGain(anAnalysis)
{
    for (std::map<QString, TF1*>::iterator iu = theMeanFitFuncCal_.begin(); iu != theMeanFitFuncCal_.end(); ++iu)
    {
        aStepCorrection_[iu->first] = 15./iu->second->GetParameter(1)*6.;
        for (int i = 0; i < maxRows_; ++i)
        {
            for (int j = 0; j < maxCols_; ++j)
            {
                if (isPixelActive_[iu->first][std::pair<int,int>(i,j)]["??. Vtrim vs 25. Vcal"])
                {
                    trimCorrections_[iu->first][std::pair<int,int>(i,j)] = (int)(aStepCorrection_[iu->first]*(theThreasholdBareVCal_[iu->first][std::pair<int,int>(i,j)] -iu->second->GetParameter(0) + 3*iu->second->GetParameter(1)));
                }
            }
        }
    }
}


