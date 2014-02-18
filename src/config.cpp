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

// Print the program options
ostream& operator << (ostream& o, config a)
{
  o << "Options:" << endl;
  o << "  display:\t" << a.display << endl;
  o << "  debug:\t" << a.debug << endl;
  o << "  size:\t\t(" << a.width << ", " << a.height << ")" << endl;
  o << "  simulation time:\t" << a.simulationTime << endl;
  o << "  model:\t" << a.model << endl;
  o << "  environment:\t" << a.environment << endl;
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

	  // If its a flag option, we'll know it's been set
	  args.modelConfig[key] = "";
	  // If it has a value, put it in the map
	  if ( opt->value.size() > 0 )
	    args.modelConfig[key] = (*opt).value[0];
	}

	// If the string key starts with "environment"
	if (boost::starts_with(opt->string_key, "environment-"))
	{
	  size_t delimPos = opt->string_key.find("-");
	  string key = opt->string_key.substr(delimPos+1, string::npos); 

	  // If its a flag option, we'll know it's been set
	  args.environmentConfig[key] = "";
	  // If it has a value, put it in the map
	  if ( opt->value.size() > 0 )
	    args.environmentConfig[key] = (*opt).value[0];
	}
      }
      catch(boost::bad_lexical_cast)
      {
      }

    }
  }  
}


// Parse command line and config file
int parseArguments(int argc, char **argv, config &args)
{
  // Default options
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
    ("help,h", "produce help message")
    ("config,c", po::value<string>(), "specify a configuration file")
  ;

  // Options that can be set from the command line or from a config file
  po::options_description commandLineAndConfig("Command line or config. file");
  commandLineAndConfig.add_options()
    ("display", "show display")
    ("debug", "turn on debugging")
    ("model,m", po::value<string>(&(args.model)), "specify the reproduction model to use")
    ("environment,e", po::value<string>(&(args.environment)), "specify the environment to use")
    ("size,s", po::value<string>(), "set the size of the grid")
    ("time,t", po::value<int>(&(args.simulationTime)), "simulation time")
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

  if (vm.count("display"))
    args.display = true;
  if (vm.count("debug"))
    args.debug = true;

  // Some parsing for grid size. We expect string of format "widthxheight".
  if (vm.count("size"))
  {
    string sizeString = vm["size"].as<string>();
    size_t delimPos = sizeString.find("x");
    args.width  = stoi( sizeString.substr(0, delimPos) );
    args.height = stoi( sizeString.substr(delimPos+1, string::npos) );
  }

  cout << "Model options:" << endl;
  for(map<string,string>::iterator it = args.modelConfig.begin(); it!=args.modelConfig.end(); ++it)
    cout << it->first << ":" << it->second << endl;
  cout << endl << "Environment options:" << endl;
  for(map<string,string>::iterator it = args.environmentConfig.begin(); it!=args.environmentConfig.end(); ++it)
    cout << it->first << ":" << it->second << endl;

  cout << args << endl;

  return 0;
}
