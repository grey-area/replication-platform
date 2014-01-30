struct arguments
{
  string       use;
  bool         display;
  bool         debug;
  int          width;
  int          height;
  int          generations;
  string       model;
  string       environment;
};

ostream& operator << (ostream&, arguments);
arguments parseArguments(int, char**);
