struct arguments
{
  string       use;
  bool         display;
  int          profiling;
  int          width;
  int          height;
  int          generations;
  string       model;
};

ostream& operator << (ostream&, arguments);
arguments parseArguments(int, char**);
