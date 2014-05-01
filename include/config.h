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

void setResultsDir(config&);
ostream& operator << (ostream&, config);
void writeConfigFile(config&);
int parseArguments(int, char**, config&);

#endif
