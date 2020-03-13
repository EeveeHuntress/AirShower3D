#include "cli.h"

cli::cli(int uargc, char* uargv[])
    : action(eNoAction)
    , stopFlag(false)
{
    readCommandLineArguments(uargc, uargv);
    evaluateCommandLineArguments();

    if((action & ePrintUsage) == ePrintUsage)   printUsage();
    if((action & ePrintBadFile) == ePrintBadFile) printBadFile();
    if((action & ePrintREADME) == ePrintREADME) printREADME();
    if((action & eOverrideConfig) == eOverrideConfig) overrideConfig();
    if((action & eSetStopFlag) == eSetStopFlag) setStopFlag();
}

cli::~cli() {};

bool
cli::continueRun() { return !stopFlag; }

std::string
cli::getConfigFile() { return configFile; }

bool
cli::checkFile()
{
    bool fileok;
    std::ifstream ts(argv[2].c_str(), std::ios_base::in);
    if(ts.is_open())
	fileok = true;
    else
	fileok = false;
    
    return fileok;
}

void
cli::readCommandLineArguments(int uargc, char* uargv[])
{
    argc = uargc;
    for(int i = 0; i < argc; ++i)
	argv.push_back(std::string(uargv[i]));
}

void
cli::evaluateCommandLineArguments()
{
    switch(argc)
    {
    case 0:
    case 1:
        break;
    case 2 :
	if(argv[1] == "--help")
	{
	    action = action | ePrintREADME;
	    action = action | eSetStopFlag;
	}
	else if(argv[1] == "-h")
	{
	    action = action | ePrintUsage;
	    action = action | eSetStopFlag;
	}
	else
	{
	    action = action | ePrintUsage;
	    action = action | eSetStopFlag;
	}
	break;
    case 3 :
	if(argv[1] == "-c")
	{
	    if(!checkFile())
	    {
		action = action | ePrintBadFile;
		action = action | eSetStopFlag;
	    }
	    else
	    {
		action = action | eOverrideConfig;
	    }
	}
	else
	{
	    action = action | ePrintUsage;
	    action = action | eSetStopFlag;
	}   
	break;
    default:
	action = action | ePrintUsage;
	action = action | eSetStopFlag;
    }
}

void
cli::printUsage()
{
    std::cout << std::endl;
    std::cout << "Usage: airshower3d [OPTION] [configfile]" << std::endl << std::endl;
    std::cout << "  OPTION may be" << std::endl;
    std::cout << std::endl;
    std::cout << "  -c <configfile>" << std::endl;
    std::cout << "                        overrides the default configuration. " << std::endl;
    std::cout << "                        <configfile> should be either the absolute" << std::endl;
    std::cout << "                        or the relative location of the user-defined" << std::endl;
    std::cout << "                        config file" << std::endl;
    std::cout << "  -h" << std::endl;
    std::cout << "                        show this message." << std::endl;
    std::cout << "  --help" << std::endl;
    std::cout << "                        shows the instructions from the README file" << std::endl;
    std::cout << std::endl;
}

void
cli::printBadFile()
{
    std::cerr << "[ERROR]: The given file (i.e. '" << argv[2] << "') does not exist!" << std::endl;
    std::cerr << "  Aborting now..." << std::endl;
}

void
cli::printREADME()
{
    QFile readme(":/rdm/README");
    QTextStream in(&readme);

    if(readme.open(QFile::ReadOnly | QFile::Text))
    {
        std::cout << std::endl << "========================= README begin =========================" << std::endl << std::endl;
        std::cout << in.readAll().toUtf8().constData() << std::endl;
        std::cout << std::endl << "========================= README end ===========================" << std::endl << std::endl;
    }
    else
        std::cout << "[ERROR]: cli.cpp: Could not open README " << std::endl;
}

void
cli::overrideConfig()
{
    configFile = argv[2];
    Config::configFileName = configFile;
}

void
cli::setStopFlag()
{
    stopFlag = true;
}
