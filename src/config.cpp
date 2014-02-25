#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <getopt.h>
#include <cstdlib>
using namespace std;

#include "config.h"

// Used to check if string starts with a given substring
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

void setConfigDir(config &args)
{
  ostringstream dirStream;
  dirStream << "model/" << args.model << "/";
  
  for (map<string, string>::iterator modelOpt = args.modelConfig.begin(); modelOpt != args.modelConfig.end(); ++modelOpt)
  {
    dirStream << "m-" << modelOpt->first << "/";
    if (modelOpt->second!="") // not flag type option, has value
      dirStream << modelOpt->second << "/";
  }

  dirStream << "env/" << args.environment << "/";

  for (map<string, string>::iterator environOpt = args.environmentConfig.begin(); environOpt != args.environmentConfig.end(); ++environOpt)
  {
    dirStream << "e-" << environOpt->first << "/";
    if (environOpt->second!="")
      dirStream << environOpt->second << "/";
  }

  dirStream << "size/" << args.width << "x" << args.height << "/";

  dirStream << "run/" << args.seed << "/";

  args.resultsConfigDir = dirStream.str();
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
  o << "model = " << a.model << endl;
  o << "environment = " << a.environment << endl;
  o << "size = " << a.width << "x" << a.height << endl;
  o << "time = " << a.simulationTime << endl;
  for (map<string, string>::iterator modelOpt = a.modelConfig.begin(); modelOpt != a.modelConfig.end(); ++modelOpt)
  {
    o << "model-" << modelOpt->first << " = ";
    if (modelOpt->second=="") // flag type option
      o << "true" << endl;
    else
    o << modelOpt->second << endl;
  }
  for (map<string, string>::iterator environOpt = a.environmentConfig.begin(); environOpt != a.environmentConfig.end(); ++environOpt)
  {
    o << "environment-" << environOpt->first << " = ";
    if (environOpt->second=="") // flag type option
      o << "true" << endl;
    else
    o << environOpt->second << endl;
  }
  return o;
}


// Given a vector of basic_options, take the unregistered ones whose string_keys begin with "model" or "environment", and put them in the modelConfig or environmentConfig
void sortUnregisteredOptions(config &args, vector<po::basic_option<char> > options)
{
  for(vector<po::basic_option<char> >::iterator opt=options.begin(); opt!=options.end(); ++opt)
  {
    if (opt->unregistered)
    {

      try
      {
	// If the string key starts with "model"
	if (boost::starts_with(opt->string_key, "model-"))
	{
	  size_t delimPos = opt->string_key.find("-");
	  string key = opt->string_key.substr(delimPos+1, string::npos); 

	  // Only do anything if the option hasn't already been set. Has the effect that command line overrides config file, too.
	  if (not args.modelConfig.count(key) )
	  {
	    // If its a flag option, we'll know it's been set
	    args.modelConfig[key] = "";
	    // If it has a value, put it in the map
	    if ( opt->value.size() > 0 )
	      args.modelConfig[key] = (*opt).value[0];
	  }
	}

	// If the string key starts with "environment"
	if (boost::starts_with(opt->string_key, "environment-"))
	{
	  size_t delimPos = opt->string_key.find("-");
	  string key = opt->string_key.substr(delimPos+1, string::npos); 

	  if (not args.environmentConfig.count(key) )
	  {
	    // If its a flag option, we'll know it's been set
	    args.environmentConfig[key] = "";
	    // If it has a value, put it in the map
	    if ( opt->value.size() > 0 )
	      args.environmentConfig[key] = (*opt).value[0];
	  }
	}
      }
      catch(boost::bad_lexical_cast)
      {
      }

    }
  }  
}

// Create a config file based on the command-line arguments and config file arguments used in this run
void writeConfigFile(config &args)
{
  ofstream configFile;
  configFile.open(args.resultsBaseDir + args.resultsConfigDir + "config.cfg");
  configFile << args;
  configFile.close();
}
 
// Parse command line and config file
int parseArguments(int argc, char **argv, config &args)
{
  // Default options
  args.resultsBaseDir = "./results/default/";
  args.resultsConfigDir = "default/";
  args.seed    = -1;
  args.display = false;
  args.debug   = false;
  args.width   = 20;
  args.height  = 20;
  args.simulationTime = 100;
  args.model       = "BaseReplicator";
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
    ("model", po::value<string>(&(args.model)), "specify the reproduction model to use")
    ("environment", po::value<string>(&(args.environment)), "specify the environment to use")
    ("size", po::value<string>(), "set the size of the grid")
    ("time", po::value<int>(&(args.simulationTime)), "simulation time")
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
    // Sort unregistered command line options destined for the model/environment
    sortUnregisteredOptions(args, parsed.options);

    // Parse arguments from config file
    if (vm.count("config"))
    {
      ifstream configFileStream(vm["config"].as<string>());
      po::parsed_options parsed = po::parse_config_file(configFileStream, commandLineAndConfig, true);
      po::store(parsed, vm);
      configFileStream.close();
      // Sort unregistered config options destined for the model/environment
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

  if (args.resultsBaseDir=="./results/default/")
    cout << "Warning: results directory not set, putting results in ./results/default." << endl;

  if (vm.count("display"))
    args.display = true;
  if (vm.count("debug"))
    args.debug = true;

  if (vm.count("dir"))
    args.resultsBaseDir = string("./results/") + vm["dir"].as<string>() + "/";

  // Some parsing for grid size. We expect string of format "widthxheight".
  if (vm.count("size"))
  {
    string sizeString = vm["size"].as<string>();
    size_t delimPos = sizeString.find("x");
    args.width  = stoi( sizeString.substr(0, delimPos) );
    args.height = stoi( sizeString.substr(delimPos+1, string::npos) );
  }

  /*cout << "Model options:" << endl;
  for(map<string,string>::iterator it = args.modelConfig.begin(); it!=args.modelConfig.end(); ++it)
    cout << it->first << ":" << it->second << endl;
  cout << endl << "Environment options:" << endl;
  for(map<string,string>::iterator it = args.environmentConfig.begin(); it!=args.environmentConfig.end(); ++it)
  cout << it->first << ":" << it->second << endl;*/

  cout << args << endl;

  return 0;
}
