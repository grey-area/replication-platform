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


// Put some brand new (orphan) entities on the grid. Periodically called.
void newEntities()
{
  for(int j=0;j<args.height; j++)
  {
    grid[0][j] = newModel(args.model, args.debug);
    grid[0][j]->state = START;
    grid[0][j]->newEntity(args.debug);
  }
}


// Send the child's body specification to the task environment and place the child on the grid
void placeChild(BaseReplicator *child, int i, int j)
{
  // TODO send body specification to the task environment

  // Put the child in a random cell neighbouring the parent
  int x = mod((i+rand()%3-1), args.width);
  int y = mod((j +rand()%3-1), args.height);
  // TODO What do I want to print?
  if (args.debug)
    cout << "\tPlacing child " << child->id << " at (" << x << "," << y << ")" << endl;
  // If the cell is already occupied, delete the entity there
  if (grid[x][y])
  {
    // TODO what do I want to print?
    if (args.debug)
      cout << "\tDeleting " << grid[x][y]->id << " to make room" << endl;
    deleteModel(args.model, grid[x][y], args.debug);
  }
  grid[x][y] = child;
}


// Initialization
void init(int argc, char **argv)
{
  // TODO: remember I've fixed the seed here
  srand(0);

  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  args = parseArguments(argc, argv);
  cout << args;
  
  // TODO: set these from command line / config file?
  BaseReplicator::modelName = args.model;
  BaseReplicator::alphabetSize = 31;
  BaseReplicator::initialDataSize = 10;
  BaseReplicator::minDataSize = 1;
  BaseReplicator::maxDataSize = 200;

  // Initialize a 2D grid of pointers to replicator objects
  grid = new BaseReplicator**[args.width];
  for(int i=0; i<args.width; i++)
  {
    grid[i] = new BaseReplicator*[args.height];
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }

  // Put some brand new (orphan) entities on the grid
  newEntities();
}


// Main loop
int loop(int generation)
{
  int headCount = 0;
  int totalFitness = 0;

  // Every so often, add new (orphan) entities to one line of the grid
  if (generation%50==0)
    newEntities();

  // Call update on each replicator periodically
  // TODO: change mechanism? (Instead of looping over all...?)
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j]) // If there is an entity there
      {
	headCount++;
	totalFitness += grid[i][j]->fitness;

	// TODO: this is a very simple way of rewarding higher fitness (tournament selection between x,y and x-1,y) Replace.
	int i2 = mod((i-1), args.width);
	int ip = (grid[i2][j] and grid[i2][j]->fitness > grid[i][j]->fitness ? i2 : i);
	BaseReplicator *entity = grid[ip][j];

	// TODO: what do I want to be printed when debugging?
	if (args.debug)
	  cout << "Updating " << ip << "," << j << endl;

	// Update the entity. See BaseReplicator.cpp to see update cycle. Most time will be spent calling the decode function
	// which is implemented by a subclass of BaseReplicator
	entity->update(args.debug);

	// If the entity has a non-embryonic child (has reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseReplicator *child = entity->child;
	  entity->child = NULL;

	  // Send the child's body specification to the task environment, and place the child on the grid
	  placeChild(child, ip, j);
	}

      } // end of if entity exists check
    } // end of cell update
  } // end of row update

  // TODO what do we want to print/output?
  if(generation%5==0)
    cout << generation << "\t" << headCount << "\t" << totalFitness/(float)headCount << endl;

  // TODO: Periodically save current state and data
}

// Clean up memory
void cleanUp()
{
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j])
      {
        deleteModel(args.model, grid[i][j], args.debug);
      }
    }
    delete [] grid[i];
  }
  delete [] grid;
}


int main(int argc, char **argv)
{
  // Parse arguments and initialize grid
  init(argc, argv); 

  // Enter the main loop
  for(int generation=0; generation<args.generations; generation++)
    loop(generation);

  // Free allocated memory
  cleanUp();
  return 0;
}
