#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <map>
#include <fstream>
#include <sqlite3.h>

struct config
{
  string       resultsBaseDir;
  string       resultsDir;
  string       dbString;
  int          recordLevel;

  int          seed;
  int          pid;

  int          debug;
  int          width;
  int          height;

  int          spawnRate;
  int          totalFunctionEvaluations;

  string       organismModuleName;
  map <string, string> organismArgs;
  string       environmentModuleName;
  map <string, string> environmentArgs;
};

class BaseOrganism; // Forward declarations so I can have pointers to these objects without including the .hs (circular includes)
class BaseEnvironment;

struct globalVars
{
  BaseEnvironment *environment;
  std::vector <std::vector <std::vector <BaseOrganism*> > > grids;
  int gridIndex;

  int nextID;
  int generation;

  ofstream dataFile;
  sqlite3 *db;
  char *sqlErrorMsg;
};

void setResultsDir(config&);
ostream& operator << (ostream&, config);
void writeConfigFile(config&);
int parseArguments(int, char**, config&);

#endif
