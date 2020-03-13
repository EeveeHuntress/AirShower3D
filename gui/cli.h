#ifndef CLI_H
#define CLI_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <QFile>
#include <QTextStream>

#include "gui/config.h"

enum eAction
{
    eNoAction       = 0,
    ePrintUsage     = (1 << 0),
    ePrintBadFile   = (1 << 1),
    ePrintREADME    = (1 << 2),
    eOverrideConfig = (1 << 3),
    eSetStopFlag    = (1 << 4)
};

class cli
{
public:
    cli(int uargc, char* uargv[]);
    ~cli();

    bool continueRun();
    std::string getConfigFile();

private:
    int argc;
    std::vector<std::string> argv;
    std::string configFile;
    int action;
    bool stopFlag;
    
    bool checkFile();
    void readCommandLineArguments(int uargc, char* uargv[]);
    void evaluateCommandLineArguments();
    void printUsage();
    void printBadFile();
    void printREADME();
    void overrideConfig();
    void setStopFlag();
};

#endif
