#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <getopt.h>
#include <cstdlib>
using namespace std;

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


// Given a vector of basic_options, take the unregistered ones whose string_keys begin with "dev" or "env", and put them in the devArgs or envArgs
void sortUnregisteredOptions(config &args, vector<po::basic_option<char> > options)
{
  for(vector<po::basic_option<char> >::iterator opt=options.begin(); opt!=options.end(); ++opt)
  {
    if (opt->unregistered)
    {

      // If the string key starts with "dev-"
      if ( (opt->string_key).substr(0,4) == "dev-" )
      {
	size_t delimPos = opt->string_key.find("-");
	string key = opt->string_key.substr(delimPos+1, string::npos); 

	// Only do anything if the option hasn't already been set. Has the effect that command line overrides config file, too.
	if (not args.devArgs.count(key) )
	{
	  // If its a flag option, we'll know it's been set
	  args.devArgs[key] = "";
	  // If it has a value, put it in the map
	  if ( opt->value.size() > 0 )
	    args.devArgs[key] = (*opt).value[0];
	}
      }

      // If the string key starts with "env-"
      if ( (opt->string_key).substr(0,4) == "env-" )
      {
	size_t delimPos = opt->string_key.find("-");
	string key = opt->string_key.substr(delimPos+1, string::npos); 

	if (not args.envArgs.count(key) )
	{
	  // If its a flag option, we'll know it's been set
	  args.envArgs[key] = "";
	  // If it has a value, put it in the map
	  if ( opt->value.size() > 0 )
	    args.envArgs[key] = (*opt).value[0];
	}
      }

    }
  }  
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

  // Options that can only be set from the command line
  po::options_description commandLineOnly("Command line only");
  commandLineOnly.add_options()
    ("help", "produce help message")
    ("config", po::value<string>(), "specify a configuration file")
  ;

  // Options that can be set from the command line or from a config file
  po::options_description commandLineAndConfig("Command line or config. file");
  commandLineAndConfig.add_options()
    ("dir", po::value<string>(), "set the results directory")
    ("seed", po::value<int>(&(args.seed)), "set seed. -1 = time(NULL)")
    ("display", "show display")
    ("debug", "turn on debugging")
    ("developmentMechanism", po::value<string>(&(args.developmentMechanism)), "specify the development mechanism to use")
    ("environment", po::value<string>(&(args.environment)), "specify the environment to use")
    ("size", po::value<string>(), "set the size of the grid")
    ("spawnRate", po::value<int>(&(args.spawnRate)), "set the rate at which new (orphan) organisms are introduced. Introduce one row of new organisms every spawnRate*populationSize function evaluations.")
    ("averagingWindowLength", po::value<int>(&(args.averagingWindowLength)), "set the window length for the window averaged results")
    ("time", po::value<int>(&(args.totalFunctionEvaluations)), "simulation time")
  ;
  po::options_description allOptions;
  allOptions.add(commandLineOnly).add(commandLineAndConfig);

  po::variables_map vm;
  try
  {
    // Parse command line arguments
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(allOptions).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);
    // Sort unregistered command line options destined for the dev. mechanism/environment
    sortUnregisteredOptions(args, parsed.options);

    // Parse arguments from config file
    if (vm.count("config"))
    {
      ifstream configFileStream(vm["config"].as<string>());
      po::parsed_options parsed = po::parse_config_file(configFileStream, commandLineAndConfig, true);
      po::store(parsed, vm);
      configFileStream.close();
      // Sort unregistered config options destined for the dev. mechanism/environment
      sortUnregisteredOptions(args, parsed.options);
    }
  }
  catch(po::error& e) 
  { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << allOptions << std::endl; 
    return 1; 
  } 
  po::notify(vm);    

  if (vm.count("help"))
  {
    cout << allOptions  << "\n";
    return 1;
  }

  if (vm.count("display"))
    args.display = true;
  if (vm.count("debug"))
    args.debug = true;

  if (vm.count("dir"))
    args.resultsBaseDir = string("./results/") + vm["dir"].as<string>() + "/";

  if (args.resultsBaseDir=="./results/default/")
    cout << "Warning: results directory not set, putting results in ./results/default." << endl;

  // Some parsing for grid size. We expect string of format "widthxheight".
  if (vm.count("size"))
  {
    string sizeString = vm["size"].as<string>();
    size_t delimPos = sizeString.find("x");
    args.width  = stoi( sizeString.substr(0, delimPos) );
    args.height = stoi( sizeString.substr(delimPos+1, string::npos) );
  }

  cout << args << endl;

  return 0;
}
