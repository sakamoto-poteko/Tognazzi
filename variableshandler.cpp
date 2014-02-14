#include "variableshandler.h"
#include "filereader.h"

#include "QRegExp"

variablesHandler::variablesHandler(fileReader *filereader, std::string fileName)
{
        theReader_ = filereader;
        fileName_ = fileName;
}

//--------------------------------------------------------------------
void variablesHandler::fillFile ()
{
    fileName_ = getEnvPath("TOGNAZZIOUTPUTDIR").toStdString() + "/" + fileName_;

    theFile_ = TFile::Open(fileName_.c_str(), "RECREATE");
 }

//--------------------------------------------------------------------
QString variablesHandler::getEnvPath(QString environmentName)
{
    QString environmentValue = getenv(environmentName.toStdString().c_str()) ;

    if(environmentValue.isEmpty())
    {
        environmentValue = "./" ;
        std::stringstream ss;
        ss //<< ACRed << ACBold
           << "WARNING: "
           //<< ACPlain
           << "environment variable "
           << environmentName.toStdString()
           << " is undefined. Assuming "
           << environmentValue.toStdString()
           << " as its value." ;
        std::cout << ss.str() << std::endl;
    }
    if(environmentValue.at(environmentValue.size()-1) != '/') environmentValue += '/';
    return environmentValue ;
}

