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


ostream& operator << (ostream& o, config a)
{
  o << "Config:" << endl;
  o << "  display:\t" << a.display << endl;
  o << "  debug:\t" << a.debug << endl;
  o << "  size:\t\t(" << a.width << ", " << a.height << ")" << endl;
  o << "  simulation time:\t" << a.simulationTime << endl;
  o << "  model:\t" << a.model << endl;
  o << "  environment:\t" << a.environment << endl;
}


int parseArguments(int argc, char **argv, config &args)
{
  args = {false, false, 20, 20, 100, "BaseModel", "BaseEnvironment"};

  namespace po = boost::program_options;

  // TODO: should I be using these configFile and sizeString variables, or does program_options provide a way?
  // Should I be using my own struct for arguments?
  // How best to pass arguments to the modules?

  string configFile;
  po::options_description commandLineOnly("Command line only");
  commandLineOnly.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(&configFile), "specify a configuration file")
  ;

  string sizeString;
  po::options_description commandLineAndConfig("Command line or config. file");
  commandLineAndConfig.add_options()
    ("display", "show display")
    ("debug", "turn on debugging")
    ("model,m", po::value<string>(&(args.model)), "specify the reproduction model to use")
    ("environment,e", po::value<string>(&(args.environment)), "specify the environment to use")
    ("size,s", po::value<string>(&sizeString), "set the size of the grid")
    ("time,t", po::value<int>(&(args.simulationTime)), "simulation time")
  ;
  po::options_description allOptions;
  allOptions.add(commandLineOnly).add(commandLineAndConfig);

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, allOptions), vm);
    po::notify(vm);

    if (vm.count("config"))
    {
      ifstream configFileStream(configFile);
      po::store(po::parse_config_file(configFileStream, commandLineAndConfig), vm);
      configFileStream.close();
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

  if (vm.count("size"))
  {
    size_t delimPos = sizeString.find("x");
    args.width  = stoi( sizeString.substr(0, delimPos) );
    args.height = stoi( sizeString.substr(delimPos+1, string::npos) );

  }

  cout << args << endl;

  return 0;
}
