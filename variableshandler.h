#ifndef VARIABLESHANDLER_H
#define VARIABLESHANDLER_H

#include <map>
#include <string>
#include "TTree.h"
#include "filereader.h"

class variablesHandler
{
public:
    variablesHandler(fileReader * filereader, std::string fileName);
    ~variablesHandler(){;}

    void fillFile ();

    static QString getEnvPath(QString environmentName);

private:
    fileReader * theReader_;
    std::string  fileName_ ;
    TFile      * theFile_  ;
};

#endif // VARIABLESHANDLER_H
