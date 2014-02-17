#include <iostream>
#include <string>
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
  args = {false, false, 100, 100, 100, "BaseModel", "BaseEnvironment"};

  namespace po = boost::program_options;

  string sizeString;
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "produce help message")
    ("config,c", po::value<string>(), "specify a configuration file")
    ("display", "show display")
    ("debug", "turn on debugging")
    ("model,m", po::value<string>(&(args.model))->default_value("BaseReplicator"), "specify the reproduction model to use")
    ("environment,e", po::value<string>(&(args.environment))->default_value("BaseEnvironment"), "specify the environment to use")
    ("size,s", po::value<string>(&sizeString)->default_value("20x20"), "set the size of the grid")
    ("time,t", po::value<int>(&(args.simulationTime))->default_value(100))
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help"))
  {
    cout << desc << "\n";
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
