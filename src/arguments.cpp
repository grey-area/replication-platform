#include <iostream>
#include <cstdio>
#include <getopt.h>
#include <cstdlib>
//#include <ctype.h>
using namespace std;

#include "arguments.h"


ostream& operator << (ostream& o, arguments a)
{
  o << "Arguments:" << endl;
  o << "  display:\t" << a.display << endl;
  o << "  profiling:\t" << a.profiling << endl;
  o << "  size:\t\t(" << a.width << ", " << a.height << ")" << endl;
  o << "  generations:\t" << a.generations << endl;
  o << "  model:\t" << a.model << endl;
}

arguments parseArguments(int argc, char **argv)
{
  string useString = "Use  main [-d <0/1> -p -s <width>x<height> -g <generations> -m <model>]\n\n-d <0/1>\t\t- Display on/off.\n-p\t\t\t- Profiling.\n-s <width>x<height>\t- Set the size of the replicator grid.\n-g <generations>\t- Set the number of generations to run the simulator for.\n-m <model>\t\t- Specify the name of the replication model to use.";  

  arguments args = {useString, false, 0, 100, 100, 100, "BaseModel"};

  int opt;
  while ((opt = getopt(argc, argv, "d:ps:g:m:h")) != EOF)
  {
    // To do: catch problems here
    switch(opt)
    {
    case 'd':
      int d;
      if (sscanf(optarg, "%d", &d) != 1) {};
      args.display = (d!=0); break;
    case 'p': args.profiling++; break;
    case 's':
      if (sscanf(optarg, "%dx%d", &args.width, &args.height) != 2) {};
      args.width =  max(args.width,  1);
      args.height = max(args.height, 1);
      break;
    case 'g': 
      if (sscanf(optarg, "%d", &args.generations) != 1) {};
      args.generations = max(args.generations,0); break;
    case 'm': args.model = optarg; break;
    case 'h': cout << args.use << endl; exit(EXIT_SUCCESS); break;
    case '?': cout << args.use << endl; exit(EXIT_FAILURE); break;
      default: ;
    }
  }

  return args;
}
