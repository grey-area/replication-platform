#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <getopt.h>
#include <cstdlib>
using namespace std;

#include "config.h"
#include "boost/program_options.hpp" 


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

// Parse command line and config file
int parseArguments(int argc, char **argv, config &args)
{
  args = {false, false, 20, 20, 100, "BaseModel", "BaseEnvironment"};

  namespace po = boost::program_options;

  // Should I be using my own struct for arguments?
  // How best to pass arguments to the modules?

  po::options_description commandLineOnly("Command line only");
  commandLineOnly.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(), "specify a configuration file")
  ;

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
    // Take command line arguments
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(allOptions).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);

    // Collect unrecognized arguments and their values from the command line
    /*for(int i=0; i<parsed.options.size(); ++i)
    {
      if (parsed.options[i].unregistered)
	cout << parsed.options[i].string_key << ":" << parsed.options[i].value[0] << endl;
	}*/
    vector<string> unrecognized = collect_unrecognized(parsed.options, po::exclude_positional);

    // Take arguments from config file
    if (vm.count("config"))
    {
      ifstream configFileStream(vm["config"].as<string>());
      po::parsed_options parsed = po::parse_config_file(configFileStream, commandLineAndConfig, true);
      po::store(parsed, vm);
      configFileStream.close();

      // Collect unrecognized arguments from the config file
      vector<string> unrecognizedConfig = collect_unrecognized(parsed.options, po::exclude_positional);
      unrecognized.insert(unrecognized.end(), unrecognizedConfig.begin(), unrecognizedConfig.end());
    }

    for (vector<string>::iterator it = unrecognized.begin(); it != unrecognized.end(); ++it)
      cout << *it << ", ";
    cout << endl;


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
