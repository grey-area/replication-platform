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
  dirStream << "dev/" << args.organismModuleName << "/";
  
  for (map<string, string>::iterator devOpt = args.organismArgs.begin(); devOpt != args.organismArgs.end(); ++devOpt)
  {
    dirStream << "m-" << devOpt->first << "/";
    if (devOpt->second!="") // not flag type option, has value
      dirStream << devOpt->second << "/";
  }

  dirStream << "env/" << args.environmentModuleName << "/";

  for (map<string, string>::iterator environOpt = args.environmentArgs.begin(); environOpt != args.environmentArgs.end(); ++environOpt)
  {
    dirStream << "e-" << environOpt->first << "/";
    if (environOpt->second!="")
      dirStream << environOpt->second << "/";
  }

  dirStream << "size/" << args.width << "x" << args.height << "/";
  dirStream << "spawnRate/" << args.spawnRate << "/";

  dirStream << "run/" << args.pid << "-" << args.seed << "/";

  args.resultsDir = dirStream.str();

  args.dbString = args.resultsBaseDir + args.resultsDir + "population.db";
}

void serialize(ostream &o, globalVars &global)
{
  o << global.gridIndex << endl;
  o << global.nextID << endl;
  o << global.generation << endl;

  // TODO environment serialization
  //global.environment.baseSerialize(o);
  //global.environment.serialize(o);

  // TODO organisms serialization
  // TODO loop over grid, serializing organisms

  // TODO deserialization

  // TODO how change some settings on deserialization?
}

// Print the program options, in the same format as the config file
ostream& operator << (ostream& o, config a)
{
  cout.setf(ios::boolalpha);
  o << "# Options:" << endl;
  if (a.seed==-1)
    o << "# seed = time(NULL)" << endl;
  o << "seed = " << a.seed << endl;
  if (a.debug)
    o << "debug" << endl;
  o << "recordLevel = " << a.recordLevel << endl;
  o << "organismModuleName = " << a.organismModuleName << endl;
  o << "environmentModuleName = " << a.environmentModuleName << endl;
  o << "size = " << a.width << "x" << a.height << endl;
  o << "time = " << a.totalFunctionEvaluations << endl;
  o << "spawnRate = " << a.spawnRate << endl;
  for (map<string, string>::iterator devOpt = a.organismArgs.begin(); devOpt != a.organismArgs.end(); ++devOpt)
  {
    o << "org-" << devOpt->first << " = ";
    if (devOpt->second=="") // flag type option
      o << "true" << endl;
    else
    o << devOpt->second << endl;
  }
  for (map<string, string>::iterator environOpt = a.environmentArgs.begin(); environOpt != a.environmentArgs.end(); ++environOpt)
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
  string filename = args.resultsBaseDir + args.resultsDir + "config.cfg";
  configFile.open(filename.c_str());
  configFile << args;
  configFile.close();
}
 

// Parse command line and config file
int parseArguments(int argc, char **argv, config &args)
{
  // Default options
  args.resultsBaseDir = "./results/default/";
  args.resultsDir     = "default/";
  args.seed           = -1;
  args.recordLevel    = 0;
  args.debug          = false;
  args.width          = 20;
  args.height         = 20;
  args.spawnRate      = 10;
  args.totalFunctionEvaluations = 100;
  args.organismModuleName       = "BaseOrganism";
  args.environmentModuleName    = "BaseEnvironment";

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
	    else
	    {
	      int loc = line.find('=');
	      string key = line.substr(0,loc-1);
	      string value = line.substr(loc+2,string::npos);
	      if (key == "seed")
		args.seed = atoi(value.c_str());
	      else if (key == "recordLevel")
		args.recordLevel = atoi(value.c_str());
	      else if (key == "organismModuleName")
		args.organismModuleName = value;
	      else if (key == "environmentModuleName")
		args.environmentModuleName = value;
	      else if (key == "size")
	      {
		size_t delimPos = value.find("x");

		string width_string = value.substr(0, delimPos);
		stringstream ss_w(width_string);
		ss_w >> args.width;

		string height_string = value.substr(delimPos+1, string::npos);
		stringstream ss_h(height_string);
		ss_h >> args.height;

	      }
	      else if (key == "spawnRate")
		args.spawnRate = atoi(value.c_str());
	      else if (key == "time")
		args.totalFunctionEvaluations = atoi(value.c_str());
	      else if (key.substr(0,4) == "org-")
	      {
		if (value == "true")
		  args.organismArgs[key.substr(4, string::npos)] = "";
		else
		  args.organismArgs[key.substr(4, string::npos)] = value;
	      }
	      else if (key.substr(0,4) == "env-")
	      {
		if (value == "true")
		  args.environmentArgs[key.substr(4, string::npos)] = "";
		else
		  args.environmentArgs[key.substr(4, string::npos)] = value;
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
    {"recordLevel", 1, 0, 0},
    {"debug", 0, &args.debug, 1},
    {"organismModuleName", 1, 0, 0},
    {"environment", 1, 0, 0},
    {"size", 1, 0, 0},
    {"spawnRate", 1, 0, 0},
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
      else if (long_options2[option_index].name == "recordLevel")
	args.recordLevel = atoi(optarg);
      else if (long_options2[option_index].name == "organismModuleName")
        args.organismModuleName = string(optarg);
      else if (long_options2[option_index].name == "environment")
        args.environmentModuleName = string(optarg);
      else if (long_options2[option_index].name == "size")
      {
	string sizeString = string(optarg);
	size_t delimPos = sizeString.find("x");

	string width_string = sizeString.substr(0, delimPos);
	stringstream ss_w(width_string);
	ss_w >> args.width;

	string height_string = sizeString.substr(delimPos+1, string::npos);
	stringstream ss_h(height_string);
	ss_h >> args.height;

      }
      else if (long_options2[option_index].name == "spawnRate")
	args.spawnRate = atoi(optarg);
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
      if (key.substr(0,4) == "org-")
	dict = &args.organismArgs;
      else if (key.substr(0,4) == "env-")
	dict = &args.environmentArgs;
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

  if (args.recordLevel < 0)
    args.recordLevel = 0;
  else if (args.recordLevel > 2)
    args.recordLevel = 2;

  cout << args << endl;

  return 0;
}
