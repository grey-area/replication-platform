#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <map>
#include "boost/program_options.hpp"

namespace po = boost::program_options;

struct config
{
  string       resultsBaseDir;
  string       resultsConfigDir;
  int          seed;
  bool         display;
  bool         debug;
  int          width;
  int          height;
  int          simulationTime;
  string       developmentMechanism;
  string       environment;
  map <string, string> devArgs;
  map <string, string> envArgs;
};

void setConfigDir(config&);
ostream& operator << (ostream&, config);
void sortUnregisteredOptions(config&, vector<po::basic_option<char> >);
void writeConfigFile(config&);
int parseArguments(int, char**, config&);

#endif
