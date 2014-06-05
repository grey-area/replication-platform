#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <map>

struct config
{
  string       resultsBaseDir;
  string       resultsDir;
  int          seed;
  int          display;
  int          debug;
  int          width;
  int          height;
  int          spawnRate;
  int          averagingWindowLength;
  int          totalFunctionEvaluations;
  string       developmentMechanism;
  string       environment;
  map <string, string> devArgs;
  map <string, string> envArgs;
};

class BaseDevMechanism; // Forward declarations so I can have pointers to these objects without including the .hs (circular includes)
class BaseEnvironment;

struct globalVars
{
  BaseEnvironment *environment;
  std::vector <std::vector <std::vector <BaseDevMechanism*> > > grids;
  int gridIndex;
};

void setResultsDir(config&);
ostream& operator << (ostream&, config);
void writeConfigFile(config&);
int parseArguments(int, char**, config&);

#endif
