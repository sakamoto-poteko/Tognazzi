#include "filereader.h"

#include "TPad.h"
#include "TDirectory.h"
#include <TROOT.h>

#include "QRegExp"
//#include <boost/filesystem/operations.hpp>
//#include <boost/algorithm/string/trim.hpp>
//#include <boost/algorithm/string/case_conv.hpp>
//#include <boost/regex.hpp>

fileReader::fileReader()
{
    outFile_ = 0;
    isVtrimScan_ = false;
}

fileReader::fileReader(QTextEdit*& le, QProgressBar*& pb) : uMessanger(le,pb)
{
    outFile_ = 0;
    isVtrimScan_ = false;
}

fileReader::fileReader(QTextEdit *&le, QProgressBar*& pb, QStringList fileNames) : uMessanger(le,pb)
{
    for(int i = 0; i < fileNames.length(); ++i)
    {
        inputFiles_[fileNames.at(i)] = new std::ifstream(fileNames.at(i).toStdString().c_str(), std::ifstream::in );
    }
    isVtrimScan_ = false;
//    outFile_ = 0;
}

fileReader::~fileReader()
{
    if (outFile_)
    {
        outFile_->Close();
        delete outFile_;
    }
    if (!inputFiles_.empty())
    {
        for (std::map<QString, std::ifstream*>::iterator i = inputFiles_.begin(); i != inputFiles_.end(); ++i)
        {
            if (i->second) delete i->second;
        }
    }
/*
    std::map<QString, std::map<std::string, TH2F*> > histogramSum2D;
    std::map<QString, std::map<std::string, TH1F*> > histogramSum1D;
    std::map<QString, std::ifstream*> inputFiles_ ;

    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH2F*> > > histograms2D;
    std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > > histograms1D;

    for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > >::iterator it = histograms1D.begin(); it != histograms1D.end(); ++it)
    {
        for (std::map<std::pair<int, int>, std::map<std::string, TH1F*> >::iterator ik = it->second.begin(); ik != it->second.end(); ++ik)
        {
            for (std::map<std::string, TH1F*>::iterator il = ik->second.begin(); il != ik->second.end(); ++ik)
            {
                delete il->second;
            }
        }
    }
*/
}

TH2F* fileReader::getHisto2D (QString spc, int row, int col, std::string varName)
{
    if (histograms2D[spc][std::pair<int, int>(row, col)].find(varName) != histograms2D[spc][std::pair<int, int>(row, col)].end())
        return histograms2D[spc][std::pair<int, int>(row, col)][varName];
    else
    {
        std::cout << "No such histogram present" << std::endl;
        return 0;
    }
}

TH1F* fileReader::getHisto1D (QString spc, int row, int col, std::string varName)
{
    if (histograms1D[spc][std::pair<int, int>(row, col)].find(varName) != histograms1D[spc][std::pair<int, int>(row, col)].end())
        return histograms1D[spc][std::pair<int, int>(row, col)][varName];
    else
    {
        std::cout << "No such histogram present" << std::endl;
        return 0;
    }
}

TH2F* fileReader::getHistoSum2D (QString spc, std::string varName)
{
    if (histogramSum2D[spc].find(varName) != histogramSum2D[spc].end())
        return histogramSum2D[spc][varName];
    else
    {
        std::cout << "No such histogram present" << std::endl;
        return 0;
    }
}

TH1F* fileReader::getHistoSum1D (QString spc, std::string varName)
{
    if (histogramSum1D[spc].find(varName) != histogramSum1D[spc].end())
        return histogramSum1D[spc][varName];
    else
    {
        std::cout << "No such histogram present" << std::endl;
        return 0;
    }
}

TH1F* fileReader::getHistoAdc1D (QString spc, int row, int col, std::string varName)
{
    if (histograms1D[spc][std::pair<int, int>(row, col)].find(varName) != histograms1D[spc][std::pair<int, int>(row, col)].end())
        return histogramsAdc1D[spc][std::pair<int, int>(row, col)][varName];
    else
    {
        std::cout << "No such histogram present" << std::endl;
        return 0;
    }
}

bool fileReader::isVar2D (QString varName)
{
    QRegExp dim ("\\w+[.]*\\s*\\w*\\svs\\s\\w+[.]*\\s*\\w*");
    if (dim.indexIn(varName) >= 0) return true;
    else return false;
}

bool fileReader::hasVariable (std::string varName)
{
    bool hasIt = false;

    for (std::map<QString, std::map<std::string, TH1F*> >::iterator it1 = histogramSum1D.begin(); it1 != histogramSum1D.end(); ++it1)
    {
        if (it1->second.find(varName) != it1->second.end()) hasIt = true;
    }
    for (std::map<QString, std::map<std::string, TH2F*> >::iterator it2 = histogramSum2D.begin(); it2 != histogramSum2D.end(); ++it2)
    {
        if (it2->second.find(varName) != it2->second.end()) hasIt = true;
    }

    return hasIt;
}

QStringList fileReader::getListOfChips (std::string varName)
{
    QStringList list;

    if (isVar2D((QString)varName.c_str()) == false)
        for (std::map<QString, std::map<std::string, TH1F*> >::iterator it1 = histogramSum1D.begin(); it1 != histogramSum1D.end(); ++it1)
        {
            if (it1->second.find(varName) != it1->second.end()) list.push_back(it1->first);
        }

    else
        for (std::map<QString, std::map<std::string, TH2F*> >::iterator it2 = histogramSum2D.begin(); it2 != histogramSum2D.end(); ++it2)
        {
            if (it2->second.find(varName) != it2->second.end()) list.push_back(it2->first);
        }

    return list;
}
std::map<std::pair<int, int>, std::map<std::string, std::map< std::pair<int, int>, std::pair<int, int> > > > fileReader::getListOfVariables(std::map<QString, QStringList>& vars, int maxRows, int maxCols)
{
    outFile_ = TFile::Open("cacca2.root", "RECREATE");

    sendMessage("Start Parsing Files", uMessageStatus::superCazzola);
    setProgress(0);

    std::map<std::pair<int, int>, std::map<std::string, std::map<std::pair<int, int>, std::pair<int, int> > > > outputList;

    vars.clear();

    maxRows_ = maxRows;
    maxCols_ = maxCols;

    // Define regex expressions used to read information from calibration files.
    const QRegExp expIteration   ("Iteration\\s+(\\d+)\\s+\\D+\\s+reg\\s+=\\s+(\\d+)\\s*"                                       );
    const QRegExp expIteration2  ("Iteration\\s+(\\d+)\\s*x\\s*(\\d+)\\s+\\D+\\s+Reg1\\s*=\\s*(\\d+)\\s+Reg2\\s*=\\s*(\\d+)\\s*");
    const QRegExp expRowColAdc   ("r\\s+(\\d+)\\s+c\\s+(\\d+)\\s+h\\s+(\\d+)\\s+a\\s+(\\d+)\\s*"                                );
    const QRegExp expRegisters   ("-+\\s*(\\d*)\\s*Re*g*i*s*t*e*r*s*\\s*-+\\s*"                                                 );
    const QRegExp expHeaderValues("(\\w+):\\s+(\\w+[.]*\\s*\\w*)\\s*"                                                           );

    int nBins, nBins1, nBins2;
    float lowVal, lowVal1, lowVal2;
    float hiVal,  hiVal1,  hiVal2 ;
    std::string valueName, valueName1, valueName2;
    std::stringstream hName, hTitle, hDir;
    int totSteps = 0;
    int totRegister = 0;
    int totChips = maxRows_*maxCols_;
    int totFiles = inputFiles_.size();

    clearHistos();
    outFile_->DeleteAll();
    outFile_->Close();
    delete outFile_;
    outFile_ = TFile::Open("cacca.root", "RECREATE");

    QRegExp spc  ("(\\w+)_\\d+_\\d+_\\d+_t\\d+_(\\d+)_(\\d+)_(\\d+)");
    QRegExp spc2 ("_vtrim(\\d+)_(\\d+)_(\\d+)_(\\d+)");
//    VthrCalDel_dut_FBK_1E_3_FBK_A12_2_1_2013_09_22_t1450_1_0_0.txt
//    cal_trim_test_vtrim0_0_0_0.txt
//    cal_trim_test_vtrim3_0_0_0.txt
//    CaldelVthr_2013_09_19_t1445_2_0_0.txt
    QString tel, stat, plaq, chip;

    int step = 0;

    for(std::map<QString, std::ifstream*>::iterator itf = inputFiles_.begin(); itf != inputFiles_.end(); ++itf)
    {
        sendMessage((QString)"Parsing file " + itf->first, uMessageStatus::superCazzola);
        setProgress(0);

        std::string fileN = itf->first.toStdString();
        fileN = fileN.substr(fileN.find_last_of("/")+1, fileN.find_last_of(".")-20);

        if (spc.indexIn(itf->first) >= 0)
        {
            tel =  spc.cap(1);
            std::cout << "TELESCOPE: " << tel.toStdString() << std::endl;
            stat = spc.cap(2);
            plaq = spc.cap(3);
            chip = spc.cap(4);
        }
        else if (spc2.indexIn(itf->first) >= 0)
        {
            tel = "boh";
            stat = spc2.cap(2);
            plaq = spc2.cap(3);
            chip = spc2.cap(4);
            isVtrimScan_ = true;
            std::cout << "VTrim Scan: " << isVtrimScan_ << std::endl;
        }
        else
        {
            tel  = "-1";
            stat = "-1";
            plaq = "-1";
            chip = "-1";
        }

        QString SPC;
        if (tel.toStdString().find("upstream") != std::string::npos)
            SPC = "Tel_upstream___Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else if (tel.toStdString().find("downstream") != std::string::npos)
            SPC = "Tel_downstream_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else if (tel.toStdString().find("dut") != std::string::npos)
            SPC = SPC = "Tel_dut_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else if (tel.toStdString().find("dut") != std::string::npos)
            SPC = SPC = "Tel_dut0_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else if (tel.toStdString().find("dut") != std::string::npos)
            SPC = SPC = "Tel_dut1_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else if (tel == "boh")
            SPC = "TelUnknown_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip + "_VTrim" + spc2.cap(1);
//        else SPC = "TelX_Stat" + stat + "_Plaq" + plaq + "_Chip" + chip;
        else SPC = (QString)fileN.c_str();

        outFile_->mkdir(SPC.toStdString().c_str());
        std::stringstream ssv;
        for (int i = 0; i < maxRows; ++i)
            for (int j = 0; j < maxCols; ++j)
            {
                outFile_->cd(SPC.toStdString().c_str());
                ssv.str("");
                ssv << "row" << i << "col" << j;
                TDirectory* dir = 0;
                if((dir = (TDirectory*)gDirectory->FindObjectAny(ssv.str().c_str())) == 0)
                    dir = gDirectory->mkdir(ssv.str().c_str());
                dir->cd();
            }

        outFile_->cd(SPC.toStdString().c_str());
        TDirectory* dir = 0;
        if((dir = (TDirectory*)gDirectory->FindObjectAny("sum")) == 0)
            dir = gDirectory->mkdir("sum");
        dir->cd();

        std::string line = "" ;
        QString Line;

        int nLines = 0 ;
        int nRegisters = 0;
        std::map<std::string, std::string>          regAssociation;
        std::map<std::string, std::pair<int, int> > currentRegValue;

        bool header = true;
        bool headerValues = false;
        std::string headerName = "";

        while(itf->second->good())     // While there are lines left to read in the file...
        {
            std::getline(*(itf->second),line);
            Line = (QString)line.c_str();

            if(header && line.find("Iteration") != 0)
            {

                if(expRegisters.indexIn(Line) >= 0)     //fill the header
                {
                    headerValues = true;
                    nRegisters = expRegisters.cap(1).toInt();
                }
                else if(headerValues)
                {
                    if(expHeaderValues.indexIn(Line) >= 0)
                    {
                        if (expHeaderValues.cap(1) == "REG"  && nRegisters == 1)
                        {
                            headerName = expHeaderValues.cap(2).toStdString() + "|NULL";
                            regAssociation["reg" ] = headerName;
                            vars[SPC].push_back(expHeaderValues.cap(2) + (QString)"");
                            valueName = expHeaderValues.cap(2).toStdString();
                        }
                        if (expHeaderValues.cap(1) == "REG"  && nRegisters == 2)
                        {
                            headerName += expHeaderValues.cap(2).toStdString();
//                            vars[SPC].push_back(expHeaderValues.cap(2) + (QString)"");
                            valueName1 = expHeaderValues.cap(2).toStdString();
                        }
                        if (expHeaderValues.cap(1) == "REG2" && nRegisters == 2)
                        {
                            headerName += "|" + expHeaderValues.cap(2).toStdString();
                            vars[SPC].push_back((QString)valueName1.c_str() + " vs " + expHeaderValues.cap(2));
                            valueName2 = expHeaderValues.cap(2).toStdString();
                        }
                        if (expHeaderValues.cap(1) == "START" && nRegisters == 1)
                        {
                            lowVal = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "START" && nRegisters == 2)
                        {
                            lowVal1 = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "START2" && nRegisters == 2)
                        {
                            lowVal2 = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "STEP" && nRegisters == 1)
                        {
                            hiVal = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "STEP" && nRegisters == 2)
                        {
                            hiVal1 = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "STEP2" && nRegisters == 2)
                        {
                            hiVal2 = expHeaderValues.cap(2).toFloat();
                        }
                        if (expHeaderValues.cap(1) == "ITERATIONS" && nRegisters == 1)
                        {
                            nBins = expHeaderValues.cap(2).toInt();
                            hiVal = hiVal*nBins + lowVal;
                            totRegister += nBins;
                        }
                        if (expHeaderValues.cap(1) == "ITERATIONS" && nRegisters == 2)
                        {
                            nBins1 = expHeaderValues.cap(2).toInt();
                            hiVal1 = hiVal1*nBins1 + lowVal1;
                            totRegister += nBins1;
                        }
                        if (expHeaderValues.cap(1) == "ITERATIONS2" && nRegisters == 2)
                        {
                            nBins2 = expHeaderValues.cap(2).toInt();
                            hiVal2 = hiVal2*nBins2 + lowVal2;
                            totRegister += nBins2;
                        }
                        if (expHeaderValues.cap(1) == "INJECT_NUM" && nRegisters == 1)
                        {
                            pixelInjection_[SPC][valueName] = expHeaderValues.cap(2).toInt();
                        }
                        if (expHeaderValues.cap(1) == "INJECT_NUM" && nRegisters == 2)
                        {
                            pixelInjection_[SPC][(std::string)(valueName1 + " vs " + valueName2)] = expHeaderValues.cap(2).toInt();
                        }
                        if (expHeaderValues.cap(1) == "MAX_ADC")
                        {
                            for (int i = 0; i < maxRows; ++i)
                                for (int j = 0; j < maxCols; ++j)
                                {
                                    if (nRegisters == 1)
                                    {
                                        hName.str(""); hTitle.str(""); hDir.str("");
                                        hDir << "row" << i << "col" << j;
                                        outFile_->cd();
                                        outFile_->cd((SPC.toStdString() + "/" + hDir.str()).c_str());
                                        hName << "histo_" << SPC.toStdString() << "_" << valueName << "_r" << i << "c" << j;
                                        std::cout << hName.str().c_str() << std::endl;
                                        hTitle << "1D histogram of " << valueName  << " for chip " << SPC.toStdString() << ", for pixel " << i << "x" << j;
                                        histograms1D[SPC][std::pair<int, int>(i, j)][valueName] = new TH1F(hName.str().c_str(), hTitle.str().c_str(), nBins, lowVal, hiVal);
                                        histograms1D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName)]->GetXaxis()->SetTitle(valueName.c_str());
                                        histograms1D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName)]->GetYaxis()->SetTitle("Counts");
                                        hName.str(""); hTitle.str("");
                                        hName << "histoADC_" << SPC.toStdString() << "_" << valueName << "_r" << i << "c" << j;
                                        hTitle << "1D histogram of " << valueName  << " adc, for chip " << SPC.toStdString() << ", for pixel " << i << "x" << j;
                                        histogramsAdc1D[SPC][std::pair<int, int>(i, j)][valueName] = new TH1F(hName.str().c_str(), hTitle.str().c_str(), nBins, lowVal, hiVal);
                                        histogramsAdc1D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName)]->GetXaxis()->SetTitle(valueName.c_str());
                                        histogramsAdc1D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName)]->GetYaxis()->SetTitle("Adc");
                                        outFile_->cd(SPC.toStdString().c_str());
                                        isPixelActive_[SPC][std::pair<int, int>(i, j)][(std::string)(valueName)] = false;
                                    }
                                    else if (nRegisters == 2)
                                    {
                                        hName.str(""); hTitle.str(""); hDir.str("");
                                        hDir << "row" << i << "col" << j;
                                        outFile_->cd();
                                        outFile_->cd((SPC.toStdString() + "/" + hDir.str()).c_str());
                                        hName << "histo_" << SPC.toStdString() << "_"  << valueName1 << "_vs_" << valueName2 << "_r" << i << "c" << j;
                                        std::cout << hName.str().c_str() << std::endl;
                                        hTitle << "2D histogram of " << valueName1 << " vs " << valueName2  << " for chip " << SPC.toStdString() << ", for pixel " << i << "x" << j;
                                        histograms2D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName1 + " vs " + valueName2)] = new TH2F(hName.str().c_str(), hTitle.str().c_str(), nBins1, lowVal1 , hiVal1, nBins2, lowVal2, hiVal2);
                                        histograms2D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName1 + " vs " + valueName2)]->GetXaxis()->SetTitle(valueName1.c_str());
                                        histograms2D[SPC][std::pair<int, int>(i, j)][(std::string)(valueName1 + " vs " + valueName2)]->GetYaxis()->SetTitle(valueName2.c_str());
                                        outFile_->cd(SPC.toStdString().c_str());
                                        isPixelActive_[SPC][std::pair<int, int>(i, j)][(std::string)(valueName1 + " vs " + valueName2)] = false;
                                    }
                                }

                            hName.str(""); hTitle.str(""); hDir.str("");
                            hDir << "sum";
                            outFile_->cd();
                            outFile_->cd((SPC.toStdString() + "/" + hDir.str()).c_str());
                            if (nRegisters == 1)
                            {
                                hName << "histo_" << SPC.toStdString() << "_" << valueName << "_sum";
                                std::cout << hName.str().c_str() << std::endl;
                                hTitle << "1D histogram of " << valueName  << " for chip " << SPC.toStdString() << ", summmed on all pixels";
                                histogramSum1D[SPC][valueName] = new TH1F(hName.str().c_str(), hTitle.str().c_str(), nBins, lowVal, hiVal);
                                histogramSum1D[SPC][valueName]->GetXaxis()->SetTitle(valueName.c_str());
                                histogramSum1D[SPC][valueName]->GetYaxis()->SetTitle("Counts");
                                pixelActive_[SPC][valueName] = 0;
                                outFile_->cd(SPC.toStdString().c_str());
                            }
                            if (nRegisters == 2)
                            {
                                hName << "histo_" << SPC.toStdString() << "_"  << valueName1 << "_vs_" << valueName2 << "_sum";
                                std::cout << hName.str().c_str() << std::endl;
                                hTitle << "2D histogram of " << valueName1 << " vs " << valueName2  << " for chip " << SPC.toStdString() << ", summed on all pixels";
                                histogramSum2D[SPC][(std::string)(valueName1 + " vs " + valueName2)] = new TH2F(hName.str().c_str(), hTitle.str().c_str(), nBins1, lowVal1 , hiVal1, nBins2, lowVal2, hiVal2);
                                histogramSum2D[SPC][(std::string)(valueName1 + " vs " + valueName2)]->GetXaxis()->SetTitle(valueName1.c_str());
                                histogramSum2D[SPC][(std::string)(valueName1 + " vs " + valueName2)]->GetYaxis()->SetTitle(valueName2.c_str());
                                pixelActive_[SPC][(std::string)(valueName1 + " vs " + valueName2)] = 0;
                                outFile_->cd(SPC.toStdString().c_str());
                            }
                        }
                        if (nRegisters == 2) regAssociation["REGs"] = headerName;
                    }
                    else
                    {
                        header       = false;
                        headerValues = false;
                    }
                }

                totSteps = totRegister*totChips*totFiles;
            }
            else
            {
                if(line.find("Iteration") != std::string::npos)
                {
                    if (nRegisters == 1)
                    {
                        if(expIteration.indexIn(Line) >= 0)
                        {
                            currentRegValue[regAssociation["reg"]] = std::pair<int, int>(expIteration.cap(2).toInt(), -1);
                        }
                    }
                    else if (nRegisters == 2)
                        if(expIteration2.indexIn(Line) >= 0)
                        {
                            currentRegValue[regAssociation["REGs"]] = std::pair<int, int>(expIteration2.cap(3).toInt(), expIteration2.cap(4).toInt());
                        }

                }
                else if(expRowColAdc.indexIn(Line) >= 0) //fill pixel
                {
                    if (nRegisters == 1)
                    {
                        outputList[std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][regAssociation["reg"]][currentRegValue[regAssociation["reg"]]] =
                                                   std::pair<int, int> (expRowColAdc.cap(3).toInt(), expRowColAdc.cap(4).toInt());
                        histograms1D[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName]->Fill(currentRegValue[regAssociation["reg"]].first, expRowColAdc.cap(3).toInt());
                        histogramsAdc1D[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName]->Fill(currentRegValue[regAssociation["reg"]].first, expRowColAdc.cap(4).toInt());
                        histogramSum1D[SPC][valueName]->Fill(currentRegValue[regAssociation["reg"]].first, expRowColAdc.cap(3).toInt());\
                        if (isPixelActive_[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName] == false)
                        {
                            pixelActive_[SPC][valueName] += 1;
                        }
                        isPixelActive_[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName] = true;
                        ++step;
                        if (step%100 == 0)
                            setProgress((int)(step/totSteps)*100);
                    }
                    else if (nRegisters == 2)
                    {
                        outputList[std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][regAssociation["REGs"]][currentRegValue[regAssociation["REGs"]]] =
                                                       std::pair<int, int> (expRowColAdc.cap(3).toInt(), expRowColAdc.cap(4).toInt());
                        histograms2D[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName1 + " vs " + valueName2]->Fill(currentRegValue[regAssociation["REGs"]].first, currentRegValue[regAssociation["REGs"]].second, expRowColAdc.cap(3).toInt());
                        histogramSum2D[SPC][valueName1 + " vs " + valueName2]->Fill(currentRegValue[regAssociation["REGs"]].first, currentRegValue[regAssociation["REGs"]].second, expRowColAdc.cap(3).toInt());
                        if (isPixelActive_[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName1 + " vs " + valueName2] == false)
                        {
                            pixelActive_[SPC][valueName1 + " vs " + valueName2] += 1;
                        }
                        isPixelActive_[SPC][std::pair<int, int>(expRowColAdc.cap(1).toInt(), expRowColAdc.cap(2).toInt())][valueName1 + " vs " + valueName2] = true;
                        ++step;
                        if (step%100 == 0)
                            setProgress((int)(step/totSteps)*100);
                    }
                }
            }
            ++nLines ;
        }
/*
        if (nRegisters == 1)
        {
            for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > >::iterator is = histograms1D.begin(); is != histograms1D.end(); ++is)
            {
                for (int i = 0; i < maxRows; ++i)
                    for (int j = 0; j < maxCols; ++j)
                    {
                        for (std::map<std::string, TH1F*>::iterator ih = is->second[std::pair<int,int>(i,j)].begin(); ih != is->second[std::pair<int,int>(i,j)].end(); ++ih)
 //                           if (ih->second->GetEntries() > 0) pixelActive_[is->first][ih->first] += 1;
                            j++;
                    }
            }
        }
        if (nRegisters == 2)
        {
            for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH2F*> > >::iterator is = histograms2D.begin(); is != histograms2D.end(); ++is)
            {
                for (int i = 0; i < maxRows; ++i)
                    for (int j = 0; j < maxCols; ++j)
                    {
                        for (std::map<std::string, TH2F*>::iterator ih = is->second[std::pair<int,int>(i,j)].begin(); ih != is->second[std::pair<int,int>(i,j)].end(); ++ih)
//                            if (ih->second->GetEntries() > 0) pixelActive_[is->first][ih->first] += 1;
                            j++;
                    }
             }
        }
*/
        outFile_->Cd("..");
    }

    return outputList;

    setProgress(100);
}


int fileReader::stringToInt(std::string inputString)
{
    std::istringstream iss_ ;
    iss_.str(inputString)	  ;
    int n			  ;
    iss_ >> n		  ;

    return n                ;
}

void fileReader::clearHistos()
{
    for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > >::iterator iu1 = histograms1D.begin(); iu1 != histograms1D.end(); ++iu1)
        for (std::map<std::pair<int, int>, std::map<std::string, TH1F*> >::iterator it1 = iu1->second.begin(); it1 != iu1->second.end(); ++it1)
        {
            for (std::map<std::string, TH1F*>::iterator iy1 = it1->second.begin(); iy1 != it1->second.end(); ++iy1)
                if (iy1->second)
                {
                    iy1->second->Delete();
                    delete iy1->second;
//                  gPad->Clear();
                    gPad->Modified();
                    gPad->Update();
                }
            it1->second.clear();
        }
    for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > >::iterator iu1 = histogramsAdc1D.begin(); iu1 != histogramsAdc1D.end(); ++iu1)
        for (std::map<std::pair<int, int>, std::map<std::string, TH1F*> >::iterator it1 = iu1->second.begin(); it1 != iu1->second.end(); ++it1)
        {
            for (std::map<std::string, TH1F*>::iterator iy1 = it1->second.begin(); iy1 != it1->second.end(); ++iy1)
                if (iy1->second)
                {
                    iy1->second->Delete();
                    delete iy1->second;
//                  gPad->Clear();
                    gPad->Modified();
                    gPad->Update();
                }
            it1->second.clear();
        }
    for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH2F*> > >::iterator iu2 = histograms2D.begin(); iu2 != histograms2D.end(); ++iu2)
        for (std::map<std::pair<int, int>, std::map<std::string, TH2F*> >::iterator it2 = iu2->second.begin(); it2 != iu2->second.end(); ++it2)
        {
            for (std::map<std::string, TH2F*>::iterator iy2 = it2->second.begin(); iy2 != it2->second.end(); ++iy2)
                if (iy2->second)
                {
                    iy2->second->Delete();
                    delete iy2->second;
//                  gPad->Clear();
                    gPad->Modified();
                    gPad->Update();
                }
            it2->second.clear();
        }
        histograms1D.clear();
        histogramsAdc1D.clear();
        histograms2D.clear();
}

void fileReader::fitHistosAdc()
{
    std::stringstream ssName;
    for (std::map<QString, std::map<std::pair<int, int>, std::map<std::string, TH1F*> > >::iterator it = histogramsAdc1D.begin(); it != histogramsAdc1D.end(); ++it)
    {
        for (std::map<std::pair<int, int>, std::map<std::string, TH1F*> >::iterator ie = it->second.begin(); ie != it->second.end(); ++ie)
        {
            for (std::map<std::string, TH1F*>::iterator iy = ie->second.begin(); iy != ie->second.end(); ++iy)
            {
                ssName.str("");
                ssName << "fitADC_" << it->first.toStdString() << "_" << iy->first << "_r" << ie->first.first << "_c" << ie->first.second;
                fitHistosAdc1D[it->first][ie->first][iy->first] = new TF1(ssName.str().c_str(), "[0] + [1]*tanh([2]*x + [3])");
                fitHistosAdc1D[it->first][ie->first][iy->first]->SetLineColor(kRed);
                fitHistosAdc1D[it->first][ie->first][iy->first]->SetParameters(400,300,0.00004,-0.1);
                iy->second->Fit(fitHistosAdc1D[it->first][ie->first][iy->first], "Q", 0, 255);
            }
        }
    }
}

void fileReader::saveFile()
{
    outFile_->Write("",TObject::kOverwrite);
    outFile_->Save();
    outFile_->Close();
    delete outFile_;
}

void fileReader::advanceProgressBar (int value)
{
    setProgress(value);
}
