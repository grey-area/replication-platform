#include <iostream>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <sys/stat.h>

/**
   \mainpage Testing
   Blah

   \section intro_sec Introduction
   Blah

   \section install_sec Installation
   Blah
 */

using namespace std;

#include "config.h"
#include "misc.h"
#include "BaseDevMechanism.h"
#include "BaseEnvironment.h"
#include "module_handler.h"

// The grid of organisms
vector <vector <BaseDevMechanism* > > grid;
// The environment, which implements the fitness function(s)
BaseEnvironment *environment;

float populationFitnessAverage = 0.0;
int populationGestationTimeAverage = 0;
float longWindowFitnessAverage = 0.0;

// Put some random, new (orphan) organisms on the grid. Periodically called. (How frequently? parameter?)
void newOrganisms(config &args)
{
  for(int j=0;j<args.height; j++)
  {
    if (grid[0][j])
      deleteOrganism(args, grid[0][j]);
    // Ask the model_handler for a new organism
    grid[0][j] = newOrganism(args); 
    grid[0][j]->initializeOrphan(args);
  }
}

// Place the child on the grid
void placeChild(config &args, BaseDevMechanism *child, int i, int j, int &x, int &y)
{
  // Put the child in a random cell neighbouring the parent
  x = mod((i+rand()%3-1), args.width);
  y = mod((j +rand()%3-1), args.height);

  // If the cell is already occupied, delete the entity there
  if (grid[x][y])
    deleteOrganism(args, grid[x][y]);

  grid[x][y] = child;
}

// Initialization
int initialize(int argc, char **argv, config &args)
{
  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.developmentMechanism is the name of the developmental mechanism we'll use
  if ( parseArguments(argc, argv, args) )
    return 1;

  if (args.seed==-1)
    args.seed = time(NULL);
  srand(args.seed);

  // Make the directory in which the results will go
  setResultsDir(args);
  _mkdir((args.resultsBaseDir + args.resultsDir).c_str());
  // Write the current options to a config file in the results directory
  writeConfigFile(args);

  BaseDevMechanism::setArgs(args);

  // Initialize a 2D grid of pointers to (NULL) organisms
  grid.resize(args.width);
  for(int i=0; i<args.width; i++)
  {
    grid[i].resize(args.height);
    for(int j=0; j<args.height; j++)
      grid[i][j] = NULL;
  }

  // Ask the model_handler for a new environment object
  environment = newEnvironment(args);
  environment->grid = &grid;

  // Put some new (orphan) entities on the grid
  newOrganisms(args);

  return 0;
}


// Main loop
int loop(config &args, ofstream &dataFile1, ofstream &dataFile2, ofstream &reproducerFile)
{
  int headCount = 0;
  float totalScore = 0.0;
  float maxScore = 0.0;
  int totalGestationTime = 0;

  // If fitnesses are periodically updated, update them
  environment->updateFitnesses(args);

  // Call update on each organism periodically
  // TODO: change mechanism? (Instead of looping over all...?)
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (grid[i][j]) // If there is an entity there
      {
	headCount++;
	totalGestationTime += grid[i][j]->gestationTime;
	totalScore += grid[i][j]->score;
	if (grid[i][j]->score > maxScore)
	  maxScore = grid[i][j]->score;

	// Tournament selection between this entity and one of its neighbours. Selection determines who gets executed next
	int i2 = mod(i + rand()%3 - 1, args.width);
	int j2;
	if (i2==i)
	  j2 = mod(j + (rand()%2==0 ? 1 : -1), args.height);
	else
	  j2 = mod(j + rand()%3 - 1, args.height);
	int ie = i;
	int je = j;
	if (grid[i2][j2] and grid[i2][j2]->fitness > grid[i][j]->fitness)
	{
	  ie = i2;
	  je = j2;
	}
	
	BaseDevMechanism *entity = grid[ie][je];

	// Update the entity. See BaseDevMechanism.cpp for update cycle. Most time will be spent calling the decode function
	// which is implemented by a subclass of BaseDevMechanism
	entity->update(args);

	// If the entity has a non-embryonic child (has just reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseDevMechanism *child = entity->child;
	  entity->child = NULL;

	  int x,y;
	  // Place the child in the neighbourhood of the parent.
	  placeChild(args, child, ie, je, x, y);
	  // Send the child's body specification to the task environment, setting its fitness.
	  environment->interpretBody(args, x, y);

	  if (child->printCount>0)
	  {
	    reproducerFile << "Function evaluations:" << environment->functionEvaluations << endl;
	    child->print(reproducerFile);
	  }

	  populationFitnessAverage += child->fitness;
	  populationGestationTimeAverage += child->gestationTime;
	  longWindowFitnessAverage += child->fitness;



	  // # function evaluations is only ever updated inside this block. Do all time-related things here.
	  // Output population average fitness and average gestation time
	  if ((environment->functionEvaluations) % (args.width*args.height) == 0)
	  {
	    dataFile1 << environment->functionEvaluations << "\t" << populationFitnessAverage/(args.width*args.height) << "\t" << populationGestationTimeAverage/(float)(args.width*args.height) << endl;
	    populationFitnessAverage = 0.0;
	    populationGestationTimeAverage = 0;
	  }
	  // Output long window average fitness
	  if ((environment->functionEvaluations) % args.averagingWindowLength == 0)
	  {
	    dataFile2 << environment->functionEvaluations - args.averagingWindowLength/2 << "\t" << longWindowFitnessAverage/(float)args.averagingWindowLength << endl;
	    longWindowFitnessAverage = 0.0;
	  }
	  if ((environment->functionEvaluations) % 20000 == 0)
	  {
	    if (grid[0][0])
	      grid[0][0]->printCount=10;
	  }

	  // Every so often, add new (orphan) entities to one line of the grid
	  if (environment->functionEvaluations%(args.spawnRate*args.width*args.height)==args.spawnRate*args.width*args.height-1)
	    newOrganisms(args);



	}

      } // end of if entity exists check
    } // end of cell update
  } // end of row update

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
        deleteOrganism(args, grid[i][j]);
      }
    }
  }

  deleteEnvironment(args, environment);
}


int main(int argc, char **argv)
{
  config args;

  // Parse arguments and initialize grid
  if ( initialize(argc, argv, args) )
    return 0;

  ofstream dataFile1, dataFile2;
  dataFile1.open((args.resultsBaseDir + args.resultsDir + "time1.dat").c_str());
  dataFile2.open((args.resultsBaseDir + args.resultsDir + "time2.dat").c_str());
  ofstream reproducerFile;
  reproducerFile.open((args.resultsBaseDir + args.resultsDir + "reproducers.dat").c_str());

  // Enter the main loop
  while(environment->functionEvaluations < args.totalFunctionEvaluations)
  {
    loop(args, dataFile1, dataFile2, reproducerFile);
  }

  dataFile1.close();
  dataFile2.close();
  reproducerFile.close();

  // Free allocated memory
  cleanUp(args);
  return 0;
}
