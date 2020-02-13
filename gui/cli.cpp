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
    case 0 :
    case 1 :
	break;
    case 2 :
	if(argv[1] == "-h")
	{
	    action = action | ePrintREADME;
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
	break;
    default:
	action = action | ePrintUsage;
	action = action | eSetStopFlag;
    }
}

void
cli::printUsage()
{
    std::cout << "Usage: airshower3d [OPTION] [configfile]" << std::endl;
    std::cout << "  OPTION may be" << std::endl;
    std::cout << std::endl;
    std::cout << "  -h" << std::endl;
    std::cout << "                        shows the instructions from the README file" << std::endl;
    std::cout << "  -c <configfile>" << std::endl;
    std::cout << "                        overrides the default configfile 'configfile.cfg'. <configfile> should " << std::endl;
    std::cout << "                        be either the absolute location of the user-defined config file or the " << std::endl;
    std::cout << "                        relative path with respect to the executable" << std::endl;
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
}

void
cli::overrideConfig()
{
    configFile = argv[2];
}

void
cli::setStopFlag()
{
    stopFlag = true;
}
