#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <getopt.h>
#include <cstdlib>
using namespace std;

/*
  To add a new option, it will have to be added to:
   - the results directory string produced by setResultsDir (maybe)
   - the << operator (maybe)
   - the config parsing
   - the command line parsing
 */

#include "config.h"

void setResultsDir(config &args)
{
  ostringstream dirStream;
  dirStream << "dev/" << args.developmentMechanism << "/";
  
  for (map<string, string>::iterator devOpt = args.devArgs.begin(); devOpt != args.devArgs.end(); ++devOpt)
  {
    dirStream << "m-" << devOpt->first << "/";
    if (devOpt->second!="") // not flag type option, has value
      dirStream << devOpt->second << "/";
  }

  dirStream << "env/" << args.environment << "/";

  for (map<string, string>::iterator environOpt = args.envArgs.begin(); environOpt != args.envArgs.end(); ++environOpt)
  {
    dirStream << "e-" << environOpt->first << "/";
    if (environOpt->second!="")
      dirStream << environOpt->second << "/";
  }

  dirStream << "size/" << args.width << "x" << args.height << "/";
  dirStream << "spawnRate/" << args.spawnRate << "/";

  dirStream << "run/" << args.seed << "/";

  args.resultsDir = dirStream.str();
}

// Print the program options, in the same format as the config file
ostream& operator << (ostream& o, config a)
{
  cout.setf(ios::boolalpha);
  o << "# Options:" << endl;
  if (a.seed==-1)
    o << "# seed = time(NULL)" << endl;
  o << "seed = " << a.seed << endl;
  if (a.display)
    o << "display" << endl;
  if (a.debug)
    o << "debug" << endl;
  o << "developmentMechanism = " << a.developmentMechanism << endl;
  o << "environment = " << a.environment << endl;
  o << "size = " << a.width << "x" << a.height << endl;
  o << "time = " << a.totalFunctionEvaluations << endl;
  o << "spawnRate = " << a.spawnRate << endl;
  o << "averagingWindowLength = " << a.averagingWindowLength << endl;
  for (map<string, string>::iterator devOpt = a.devArgs.begin(); devOpt != a.devArgs.end(); ++devOpt)
  {
    o << "dev-" << devOpt->first << " = ";
    if (devOpt->second=="") // flag type option
      o << "true" << endl;
    else
    o << devOpt->second << endl;
  }
  for (map<string, string>::iterator environOpt = a.envArgs.begin(); environOpt != a.envArgs.end(); ++environOpt)
  {
    o << "env-" << environOpt->first << " = ";
    if (environOpt->second=="") // flag type option
      o << "true" << endl;
    else
    o << environOpt->second << endl;
  }
  return o;
}


// Create a config file based on the command-line arguments and config file arguments used in this run
void writeConfigFile(config &args)
{
  ofstream configFile;
  configFile.open(args.resultsBaseDir + args.resultsDir + "config.cfg");
  configFile << args;
  configFile.close();
}
 
// Parse command line and config file
int parseArguments(int argc, char **argv, config &args)
{
  // Default options
  args.resultsBaseDir = "./results/default/";
  args.resultsDir = "default/";
  args.seed    = -1;
  args.display = false;
  args.debug   = false;
  args.width   = 20;
  args.height  = 20;
  args.spawnRate = 10;
  args.averagingWindowLength = 100000;
  args.totalFunctionEvaluations = 100;
  args.developmentMechanism = "BaseDevMechanism";
  args.environment = "BaseEnvironment";

  // Set options from config file
  int c;
  int option_index = 0;
  static struct option long_options[] = {
    {"config", 1, 0, 0},
    {NULL, 0, NULL, 0}
  };
  opterr=false;

  while ((c = getopt_long(argc, argv, "",
	long_options, &option_index)) != -1)
  {
    switch (c) {
    case 0: // Long option
      if (long_options[option_index].name == "config")
      {
	ifstream ifile(optarg);
	string line;

	while (getline(ifile,line))
	{
	  if (line.at(0) != '#')
	  {
	    if (line == "debug")
	      args.debug = 1;
	    else if (line == "display")
	      args.display = 1;
	    else
	    {
	      int loc = line.find('=');
	      string key = line.substr(0,loc-1);
	      string value = line.substr(loc+2,string::npos);
	      if (key == "seed")
		args.seed = atoi(value.c_str());
	      else if (key == "developmentMechanism")
		args.developmentMechanism = value;
	      else if (key == "environment")
		args.environment = value;
	      else if (key == "size")
	      {
		size_t delimPos = value.find("x");
		args.width  = stoi( value.substr(0, delimPos) );
		args.height = stoi( value.substr(delimPos+1, string::npos) );
	      }
	      else if (key == "spawnRate")
		args.spawnRate = atoi(value.c_str());
	      else if (key == "averagingWindowLength")
		args.averagingWindowLength = atoi(value.c_str());
	      else if (key == "time")
		args.totalFunctionEvaluations = atoi(value.c_str());
	      else if (key.substr(0,4) == "dev-")
	      {
		if (value == "true")
		  args.devArgs[key.substr(4, string::npos)] = "";
		else
		  args.devArgs[key.substr(4, string::npos)] = value;
	      }
	      else if (key.substr(0,4) == "env-")
	      {
		if (value == "true")
		  args.envArgs[key.substr(4, string::npos)] = "";
		else
		  args.envArgs[key.substr(4, string::npos)] = value;
	      }
	      
	    }
	  }
	}
      }
      break;
    }
  }


  static struct option long_options2[] = {
    {"help", 0, 0, 0},
    {"dir", 1, 0, 0},
    {"seed", 1, 0, 0},
    {"display", 0, &args.display, 1},
    {"debug", 0, &args.debug, 1},
    {"developmentMechanism", 1, 0, 0},
    {"environment", 1, 0, 0},
    {"size", 1, 0, 0},
    {"spawnRate", 1, 0, 0},
    {"averagingWindowLength", 1, 0, 0},
    {"time", 1, 0, 0},
    {NULL, 0, NULL, 0}
  };

  // Set options from command line
  option_index = 0;
  optind=1;
  while ((c = getopt_long(argc, argv, "h",
	long_options2, &option_index)) != -1)
  {
    switch (c) {
    case 0: // Long option
      if (long_options2[option_index].name == "help")
      {
	cout << "HELP" << endl;
	return 1;
      }
      else if (long_options2[option_index].name == "dir")
      {
	string dir(optarg);
	args.resultsBaseDir = string("./results/") + string(optarg) + string("/");
      }
      else if (long_options2[option_index].name == "seed")
	args.seed = atoi(optarg);
      else if (long_options2[option_index].name == "developmentMechanism")
        args.developmentMechanism = string(optarg);
      else if (long_options2[option_index].name == "environment")
        args.environment = string(optarg);
      else if (long_options2[option_index].name == "size")
      {
	string sizeString = string(optarg);
	size_t delimPos = sizeString.find("x");
	args.width  = stoi( sizeString.substr(0, delimPos) );
	args.height = stoi( sizeString.substr(delimPos+1, string::npos) );
      }
      else if (long_options2[option_index].name == "spawnRate")
	args.spawnRate = atoi(optarg);
      else if (long_options2[option_index].name == "averagingWindowLength")
	args.averagingWindowLength = atoi(optarg);
      else if (long_options2[option_index].name == "time")
	args.totalFunctionEvaluations = atoi(optarg);

      break;
    case 'h':
      cout << "HELP" << endl;
      return 1;
      break;
    case '?':
      map<string,string> *dict;
      string key = string(argv[optind-1]).substr(2, string::npos);
      if (key.substr(0,4) == "dev-")
	dict = &args.devArgs;
      else if (key.substr(0,4) == "env-")
	dict = &args.envArgs;
      else
	break;
      int  loc = key.find('=');
      if (loc == string::npos)
	(*dict)[key.substr(4, string::npos)] = "";
      else
      {
	string value = key.substr(loc+1, string::npos);
	(*dict)[key.substr(4,loc-4)] = value;
      }	

      break;
    }
  }

  cout << args << endl;

  return 0;
}
