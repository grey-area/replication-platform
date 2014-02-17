#ifndef CONFIG_H
#define CONFIG_H

struct config
{
  bool         display;
  bool         debug;
  int          width;
  int          height;
  int          simulationTime;
  string       model;
  string       environment;
};

ostream& operator << (ostream&, config);
int parseArguments(int, char**, config&);

#endif
