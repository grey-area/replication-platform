#include <iostream>
#include <algorithm>
#include <time.h>

using namespace std;

#include "arguments.h"
#include "misc.h"
#include "BaseReplicator.h"
#include "modelHandler.h"


arguments args;
BaseReplicator ***grid;


// Initialization
void init(int argc, char **argv)
{
  srand(time(NULL));

  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  args = parseArguments(argc, argv);
  cout << args;
  
  // TODO: set these from command line / config file?
  BaseReplicator::modelName = args.model;
  BaseReplicator::alphabetSize = 4;
  BaseReplicator::initialDataSize = 30;
  BaseReplicator::minDataSize = 1;
  BaseReplicator::maxDataSize = 100;

  // Initialize a 2D grid of pointers to replicator objects
  grid = new BaseReplicator**[args.width];
  for(int i=0; i<args.width; i++)
  {
    grid[i] = new BaseReplicator*[args.height];
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }

  // Instantiate a single replicator at (0,0) using the replication model specified
  grid[0][0] = newModel(args.model);
  grid[0][0]->state = START;
  grid[0][0]->newEntity();

  //cout << "Created " << grid[0][0]->id << " at (0,0)" << endl;
}


// Main loop
int loop(int generation)
{
  //cout << "Pass: " << generation << endl;
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
	//cout << "\tUpdating " << entity->id << " at (" << ip << "," << j << ")" << endl;
	entity->update();

	// If the entity has a non-embryonic child (has reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseReplicator *child = entity->child;
	  entity->child = NULL;

	  // if the child has a body specification, send it to the task sequencer

	  // Put the child in a cell neighbouring the parent
	  int x = mod((ip+rand()%3-1), args.width);
	  int y = mod((j +rand()%3-1), args.height);
	  //cout << "\t\tPlacing child " << child->id << " at (" << x << "," << y << ")" << endl;
	  if (grid[x][y])
	  {
	    //cout << "\t\tReplacing " << grid[x][y]->id << endl;
	    deleteModel(args.model, grid[x][y]);
	  }
	  grid[x][y] = child;

	} // end of if reproduction event
      } // end of if entity exists check
    } // end of cell update
  } // end of row update

  //cout << "G:" << generation << " H:" << headCount << endl;

  // TODO: Periodically save current state and data
}

// Clean up memory
void cleanUp()
{
  cout << "Starting clean up" << endl;
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j])
      {
        deleteModel(args.model, grid[i][j]);
      }
    }
    delete [] grid[i];
  }
  delete [] grid;
}


int main(int argc, char **argv)
{
  init(argc, argv);

  for(int generation=0; generation<args.generations; generation++)
  {
    loop(generation);
  } 

  cleanUp();

  return 0;
}
