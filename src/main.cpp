#include <iostream>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <getopt.h>
#include <algorithm>
using namespace std;

#include "arguments.h"
#include "instantiate_replicator.h"


int main(int argc, char **argv)
{
  arguments args = parseArguments(argc, argv);
  cout << args;

  BaseReplicator*** grid = new BaseReplicator**[args.width];
  for(int i=0; i<args.width; i++)
    grid[i] = new BaseReplicator*[args.height];

  grid[0][0] = instantiate_replicator(args.model);
  grid[0][0]->update();

  for(int i=0; i<args.width; i++)
    delete [] grid[i];
  delete [] grid;

  return 0;
}
