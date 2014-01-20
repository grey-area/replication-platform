#include <iostream>
#include <algorithm>
#include <time.h>
using namespace std;

#include "arguments.h"
#include "misc.h"
#include "BaseReplicator.h"
#include "modelHandler.h"


int main(int argc, char **argv)
{
  srand(time(NULL));

  // Parse command line arguments, put result in struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  arguments args = parseArguments(argc, argv);
  cout << args;
  BaseReplicator::modelName = args.model;

  // Initialize a 2D grid of pointers to replicator objects
  BaseReplicator*** grid = new BaseReplicator**[args.width];
  for(int i=0; i<args.width; i++)
  {
    grid[i] = new BaseReplicator*[args.height];
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }


  // Instantiate a single replicator at (0,0) using the replication model specified
  grid[0][0] = newModel(args.model);
  grid[0][0]->state = START;

  // Main loop
  for(int generation=0; generation<args.generations; generation++)
  {
    int headCount = 0;

    // Call update on each replicator periodically, with frequency depending on fitness
    for(int i=0; i<args.width; i++)
    {
      for(int j=0; j<args.height; j++)
      {
	if (grid[i][j])
	{
	  headCount++;

	  // TODO: this is a very simple way of rewarding higher fitness. Replace
	  int ip = i;
	  int i2 = mod((i-1), args.width);
	  if (grid[i2][j] and grid[i2][j]->fitness > grid[i][j]->fitness)
	    ip = i2;
	  BaseReplicator *entity = grid[ip][j];

	  // Update (usually means incrementally decode genetic information)
	  entity->update();

	  // If the entity has a non-embryonic child (has reproduced)
	  if (entity->child and (entity->child)->state != EMBRYO)
	  {
	    BaseReplicator *child = entity->child;
	    entity->child = NULL;

	    // if the child has a body specification, send it to the task sequencer
	    if (child->bodySpecification)
	    {
	      // the task sequencer will turn the specification into *something* which it will then use to update the fitness of the child. This might be a one-off update, or it might be ongoing.
	    }

	    // Put the child in a cell neighbouring the parent
	    int x = mod((ip+rand()%3-1), args.width);
	    int y = mod((j +rand()%3-1), args.height);
	    if (grid[x][y])
	    {
	      deleteModel(args.model, grid[x][y]);
	    }
	    grid[x][y] = child;

	  } // end of if reproduction event
	}
      } // end of cell update
    } // end of row update
    cout << "H: " << headCount << endl;


    // TODO: Periodically save current state and data

  } // End generation

  // Clean up
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j])
        deleteModel(args.model, grid[i][j]);
    }
    delete [] grid[i];
  }
  delete [] grid;

  return 0;
}
