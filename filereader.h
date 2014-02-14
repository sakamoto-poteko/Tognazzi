#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <QStringList>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include <iostream>
#include <fstream>

#include "umessanger.h"

class pixelSelection;

class fileReader : public uMessanger
{
    friend class pixelSelection;

public:
    fileReader();
    fileReader(QTextEdit*& le, QProgressBar*& pb);
    fileReader(QTextEdit*& le, QProgressBar*& pb, QStringList fileNames);
    ~fileReader();

    typedef std::map< std::pair<int, int>, std::pair<int, int> > quadValue   ;
    typedef std::map<std::string, quadValue>                     quadValues  ;
    typedef std::map<std::pair<int, int>, quadValues>            rcQuadValues;

    std::map<std::pair<int, int>, std::map<std::string, std::map< std::pair<int, int>, std::pair<int, int> > > > getListOfVariables(std::map<QString, QStringList>& vars, int maxRows, int maxCols);

    TH1F* getHisto1D (QString spc, int row, int col, std::string varName);
    TH2F* getHisto2D (QString spc, int row, int col, std::string varName);

    TH1F* getHistoAdc1D (QString spc, int row, int col, std::string varName);

    TH1F* getHistoSum1D (QString spc, std::string varName);
    TH2F* getHistoSum2D (QString spc, std::string varName);

    QStringList getListOfChips (std::string varName);

    bool isVar2D (QString varName);
    bool hasVariable (std::string varName);
    bool isVtrimScanPossible () {return isVtrimScan_;}
    int  getNumberOfActivePixels (QString spc, std::string varName) {return pixelActive_   [spc][varName];}
    int  getNumberOfInjections   (QString spc, std::string varName) {return pixelInjection_[spc][varName];}
    bool isPixelActive (QString spc, std::string varName, int row, int col) {return isPixelActive_[spc][std::pair<int,int>(row,col)][varName];}

    std::map<QString, std::map<std::string, TH2F*> > getHistosSum2D   () {return histogramSum2D ;}//TOIMPROVE
    std::map<QString, std::map<std::string, TH1F*> > getHistosSum1D   () {return histogramSum1D ;}//TOIMPROVE
    std::map<QString, std::map<std::string, int> >   getPixelActive   () {return pixelActive_   ;}//TOIMPROVE
    std::map<QString, std::map<std::string, int> >   getPixelInjection() {return pixelInjection_;}//TOIMPROVE

    void fitHistosAdc ();

    void saveFile ();


protected:

    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, bool> > > isPixelActive_ ;
    int maxRows_;
    int maxCols_;

    std::map<QString, std::map<std::string, TH2F*> > histogramSum2D;
    std::map<QString, std::map<std::string, TH1F*> > histogramSum1D;

    std::map<QString, std::map<std::string, int> >                                  pixelActive_   ;
    std::map<QString, std::map<std::string, int> >                                  pixelInjection_;

    bool isVtrimScan_;

private slots:
        void advanceProgressBar(int value);

private:
    int stringToInt (std::string inputString);
    void clearHistos ();

    std::map<QString, std::ifstream*> inputFiles_ ;

    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH2F*> > > histograms2D;
    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > > histograms1D;

    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > > histogramsAdc1D;
    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TF1* > > > fitHistosAdc1D;

    TFile * outFile_;
};

#endif // FILEREADER_H
