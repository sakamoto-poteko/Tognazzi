#ifndef VTRIMSET_H
#define VTRIMSET_H

#include "vtrimgain.h"

class vTrimSet : public vTrimGain
{
public:
    vTrimSet(sCurveAnalysis& anAnalysis);
    ~vTrimSet(){;}

    std::map<QString, std::map<std::pair<int,int>, int> > getTrimCorrections () {return trimCorrections_;}

private:

    std::map<QString, double> aStepCorrection_;
    std::map<QString, std::map<std::pair<int,int>, int> > trimCorrections_;
};

#endif // VTRIMSET_H
