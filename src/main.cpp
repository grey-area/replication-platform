#include <iostream>
#include <algorithm>
#include <time.h>
using namespace std;

#include "arguments.h"
#include "misc.h"
#include "instantiate_replicator.h"


int main(int argc, char **argv)
{
  srand(time(NULL));

  // Parse command line arguments, put result in struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  arguments args = parseArguments(argc, argv);
  cout << args;


  // Initialize a 2D grid of pointers to replicator objects
  BaseReplicator*** grid = new BaseReplicator**[args.width];
  for(int i=0; i<args.width; i++)
  {
    grid[i] = new BaseReplicator*[args.height];
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }


  // Instantiate a single replicator at (0,0) using the replication model specified
  grid[0][0] = instantiate_replicator(args.model);


  // Main loop
  for(int generation=0; generation<args.generations; generation++)
  {
    // Call update on each replicator periodically, with frequency depending on fitness
    for(int i=0; i<args.width; i++)
    {
      for(int j=0; j<args.height; j++)
      {
	if (grid[i][j])
	{
	  grid[i][j]->baseUpdate();
	  grid[i][j]->update();
	  // If the update returns a replicator (child), put the child next to the parent.
	  // Child inherits parent's fitness initially, but this will soon be updated by the task sequencer/environment
	  if (grid[i][j]->child)
	  {
	    BaseReplicator *child = grid[i][j]->child;
	    // if the child has a body specification, send it to the task sequencer
	    // the task sequencer will turn the specification into *something* which it will then use to update the fitness of the child. This might be a one-off update, or it might be continuous.
	    if (child->bodySpecification)
	    {
	    }
	    int x = mod((i+rand()%3-1), args.width);
	    int y = mod((j+rand()%3-1), args.height);
	    if (grid[x][y])
	      delete_replicator(args.model, grid[x][y]);
	    grid[x][y] = child;
	  } // end of if reproduction event
	}
      } // end of cell update
    } // end of row update
    // Periodically (or in one go) update fitnesses
    // Periodically save current state and data
  } // End generation

  // Clean up
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j])
        delete_replicator(args.model, grid[i][j]);
    }
    delete [] grid[i];
  }
  delete [] grid;

  return 0;
}
