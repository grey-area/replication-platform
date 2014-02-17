#include <iostream>
#include <algorithm>
#include <time.h>

using namespace std;

//#include "boost/property_tree/ptree.hpp"
//#include "boost/property_tree/xml_parser.hpp"

#include "config.h"
#include "misc.h"
#include "BaseReplicator.h"
#include "BaseEnvironment.h"
#include "modelHandler.h"

vector <vector <BaseReplicator* > > grid;
BaseEnvironment *environment;

// Put some brand new (orphan) entities on the grid. Periodically called.
void newEntities(config &args)
{
  for(int j=0;j<args.height; j++)
  {
    if (grid[0][j])
      deleteModel(args, grid[0][j]);
    grid[0][j] = newModel(args);
    grid[0][j]->state = START;
    grid[0][j]->newEntity(args);
  }
}


// Send the child's body specification to the task environment and place the child on the grid
void placeChild(config &args, BaseReplicator *child, int i, int j, int &x, int &y)
{
  // Put the child in a random cell neighbouring the parent
  x = mod((i+rand()%3-1), args.width);
  y = mod((j +rand()%3-1), args.height);
  // TODO What do I want to print?
  if (args.debug)
    cout << "\tPlacing child " << child->id << " at (" << x << "," << y << ")" << endl;
  // If the cell is already occupied, delete the entity there
  if (grid[x][y])
  {
    // TODO what do I want to print?
    if (args.debug)
      cout << "\tDeleting " << grid[x][y]->id << " to make room" << endl;
    deleteModel(args, grid[x][y]);
  }
  grid[x][y] = child;
}


// Initialization
int init(int argc, char **argv, config &args)
{
  // TODO: remember I've fixed the seed here
  srand(0);

  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  if ( parseArguments(argc, argv, args) )
    return 1;
  //cout << args;
  
  // TODO: set these from command line / config file?
  // alphabet size, initial/min/max data size
  BaseReplicator::setArgs(args.model, 31, 10, 1, 100);

  // Initialize a 2D grid of pointers to replicator objects
  grid.resize(args.width);
  for(int i=0; i<args.width; i++)
  {
    grid[i].resize(args.height);
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }

  // Initialize the environment TODO: use factory
  environment = newEnvironment(args);
  environment->grid = &grid;

  // Put some brand new (orphan) entities on the grid
  newEntities(args);

  return 0;
}


// Main loop
int loop(config &args, int t)
{
  int headCount = 0;
  float totalFitness = 0.0;
  float maxFitness = 0.0;

  // Every so often, add new (orphan) entities to one line of the grid
  if (t%100==0)
    newEntities(args);

  // If fitnesses are periodically updated, update them
  environment->updateFitnesses(args);

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
	if (grid[i][j]->fitness > maxFitness)
	  maxFitness = grid[i][j]->fitness;

	// TODO: this is a very simple way of rewarding higher fitness (tournament selection between x,y and x-1,y) Replace.
	int i2 = mod((i-1), args.width);
	int ip = (grid[i2][j] and grid[i2][j]->fitness > grid[i][j]->fitness ? i2 : i);
	BaseReplicator *entity = grid[ip][j];

	// TODO: what do I want to be printed when debugging?
	if (args.debug)
	  cout << "Updating " << ip << "," << j << endl;

	// Update the entity. See BaseReplicator.cpp to see update cycle. Most time will be spent calling the decode function
	// which is implemented by a subclass of BaseReplicator
	entity->update(args);

	// If the entity has a non-embryonic child (has reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseReplicator *child = entity->child;
	  entity->child = NULL;

	  // TODO how frequently do we print this stuff?
	  if (t % 50 == 0 and i<3 and j<3)
	  {
	    //cout << "---------------------------------" << endl;
	    //entity->print();
	    //cout << endl << "Created" << endl << endl;
	    //child->print();
	  }

	  // Send the child's body specification to the task environment, and place the child on the grid
	  int x,y;
	  placeChild(args, child, ip, j, x, y);
	  environment->interpretBody(args, x, y, t);

	}

      } // end of if entity exists check
    } // end of cell update
  } // end of row update

  // TODO what do we want to print/output?
  if(t%20==0)
    cout << t << "\t" << maxFitness << "\t" << totalFitness/(float)headCount << endl;

  // TODO: Periodically save current state and data
}

// Clean up memory
void cleanUp(config &args)
{
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j])
      {
        deleteModel(args, grid[i][j]);
      }
    }
  }

  deleteEnvironment(args, environment);
}


int main(int argc, char **argv)
{
  config args;

  // Parse arguments and initialize grid
  if ( init(argc, argv, args) )
    return 0;

  // Enter the main loop
  for(int t=0; t<args.simulationTime; t++)
    loop(args, t);

  // Free allocated memory
  cleanUp(args);
  return 0;
}
