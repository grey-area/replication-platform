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

float populationFitnessAverage = 0.0;
int populationGestationTimeAverage = 0;
float longWindowFitnessAverage = 0.0;

void debugPrint(config &args, globalVars &global)
{
  for (int i=0; i<args.width; i++)
  {
    for (int j=0; j<args.height; j++)
    {
      if (global.grids[global.gridIndex][i][j])
	cout << global.grids[global.gridIndex][i][j]->id << "\t";
      else
	cout << "000000\t";
    }
    cout << endl;
  }
  cout << endl;
  for (int i=0; i<args.width; i++)
  {
    for (int j=0; j<args.height; j++)
    {
      if (global.grids[1-global.gridIndex][i][j])
	cout << global.grids[1-global.gridIndex][i][j]->id << "\t";
      else
	cout << "000000\t";
    }
    cout << endl;
  }
  cout << endl << "*********************" << endl;
}

// Put some random, new (orphan) organisms on the grid. Periodically called. (How frequently? parameter?)
void newOrganisms(config &args, globalVars &global)
{
  for(int j=0;j<args.height; j++)
  {
    // If there's already a new organism there, delete it
    if ( global.grids[1-global.gridIndex][0][j] and global.grids[1-global.gridIndex][0][j] != global.grids[global.gridIndex][0][j])
      deleteOrganism(args, global.grids[1-global.gridIndex][0][j]);
    // Ask the model_handler for a new organism
    global.grids[1-global.gridIndex][0][j] = newOrganism(args); 
    global.grids[1-global.gridIndex][0][j]->initializeOrphan(args);
  }

  //cout << "Placing new organisms" << endl;
  //debugPrint(args, global);
}

// Place the child on the grid
void placeChild(config &args, globalVars &global, BaseDevMechanism *child, int i, int j, int &x, int &y)
{
  // Put the child in a random cell neighbouring the parent (in the other grid)
  x = mod((i+rand()%3-1), args.width);
  y = mod((j +rand()%3-1), args.height);

  // If the cell is already occupied by a new organism, delete it
  if (global.grids[1-global.gridIndex][x][y] and global.grids[1-global.gridIndex][x][y] != global.grids[global.gridIndex][x][y])
    deleteOrganism(args, global.grids[1-global.gridIndex][x][y]);

  global.grids[1-global.gridIndex][x][y] = child;

  //cout << "Placing child" << endl;
  //debugPrint(args, global);
}

// Initialization
int initialize(int argc, char **argv, config &args, globalVars &global)
{
  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.developmentMechanism is the name of the developmental mechanism we'll use
  if ( parseArguments(argc, argv, args) )
    return 1;

  if (args.seed==-1)
    args.seed = time(NULL) + args.pid;
  srand(args.seed);

  // Make the directory in which the results will go
  setResultsDir(args);
  _mkdir((args.resultsBaseDir + args.resultsDir).c_str());
  // Write the current options to a config file in the results directory
  writeConfigFile(args);

  BaseDevMechanism::setArgs(args);

  // Initialize a 2D grid of pointers to (NULL) organisms
  global.grids.resize(2);
  for (int g=0; g<2; g++)
  {
    global.grids[g].resize(args.width);
    for(int i=0; i<args.width; i++)
    {
      global.grids[g][i].resize(args.height);
      for(int j=0; j<args.height; j++)
	global.grids[g][i][j] = NULL;
    }
  }
  global.gridIndex = 0;

  // Ask the model_handler for a new environment object
  global.environment = newEnvironment(args);

  // Put some new (orphan) entities on the grid
  newOrganisms(args, global);

  return 0;
}


// Main loop
int loop(config &args, globalVars &global, ofstream &dataFile1, ofstream &dataFile2, ofstream &reproducerFile)
{
  global.gridIndex = 1 - global.gridIndex;

  //cout << "Switching frame" << endl;
  //debugPrint(args, global);

  int headCount = 0;
  float totalScore = 0.0;
  float maxScore = 0.0;
  int totalGestationTime = 0;

  int placeNewOrganisms = 0;

  // If fitnesses are periodically updated, update them
  global.environment->updateFitnesses(args, global);

  // Call update on each organism periodically
  // TODO: change mechanism? (Instead of looping over all...?)
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      //cout << "cell " << i << ", " << j << endl;
      if (global.grids[global.gridIndex][i][j]) // If there is an entity there
      {
	// Make the other grid the same as this one, unless the entity on the other grid is new
	// If we do replace what was in the other grid, delete the entity
	if (global.grids[global.gridIndex][i][j] != global.grids[1-global.gridIndex][i][j])
	{
	  // If there is something in the old grid and it's not new
	  if ( global.grids[1-global.gridIndex][i][j] )
	  {
	    if ( global.grids[1-global.gridIndex][i][j]->age > 0 )
	    {
	      deleteOrganism(args, global.grids[1-global.gridIndex][i][j]);
	      global.grids[1-global.gridIndex][i][j] = global.grids[global.gridIndex][i][j];

	      //cout << "Making old frame match current frame" << endl;
	      //debugPrint(args, global);
	    }
	  }
	  else // if there isn't something in old grid 
	  {
	    global.grids[1-global.gridIndex][i][j] = global.grids[global.gridIndex][i][j];

	    //cout << "Making old frame match current frame" << endl;
	    //debugPrint(args, global);
	  }
	}

	global.grids[global.gridIndex][i][j]->age++;
	headCount++;
	totalGestationTime += global.grids[global.gridIndex][i][j]->gestationTime;
	totalScore += global.grids[global.gridIndex][i][j]->score;
	if (global.grids[global.gridIndex][i][j]->score > maxScore)
	  maxScore = global.grids[global.gridIndex][i][j]->score;

	// Tournament selection between this entity and one of its neighbours. Selection determines who gets executed next
	int i2 = mod(i + rand()%3 - 1, args.width);
	int j2;
	if (i2==i)
	  j2 = mod(j + (rand()%2==0 ? 1 : -1), args.height);
	else
	  j2 = mod(j + rand()%3 - 1, args.height);
	int ie = i;
	int je = j;
	if (global.grids[global.gridIndex][i2][j2] and global.grids[global.gridIndex][i2][j2]->fitness > global.grids[global.gridIndex][i][j]->fitness)
	{
	  ie = i2;
	  je = j2;
	}

	//cout << "update " << ie << ", " << je << endl << endl;

	BaseDevMechanism *entity = global.grids[global.gridIndex][ie][je];

	// Update the entity. See BaseDevMechanism.cpp for update cycle. Most time will be spent calling the decode function
	// which is implemented by a subclass of BaseDevMechanism
	entity->update(args, global);

	// If the entity has a non-embryonic child (has just reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseDevMechanism *child = entity->child;
	  entity->child = NULL;

	  int x,y;
	  // Place the child in the neighbourhood of the parent.
	  placeChild(args, global, child, ie, je, x, y);
	  
	  // Send the child's body specification to the task environment, setting its fitness.
	  global.environment->interpretBody(args, global, x, y);
	  // TODO: made it print out every one
	  if (global.environment->functionEvaluations>100000 and global.environment->functionEvaluations<120000)
	    //global.grids[global.gridIndex][x][y]->printCount=1;

	  if (child->printCount>0)
	  {
	    reproducerFile << "Function evaluations:" << global.environment->functionEvaluations << endl;
	    child->print(reproducerFile);
	  }

	  // TODO added this to discount drops due to function change, and added below to record probability of beneficial mutation
	  //global.environment->interpretBody(args, ie, je);
	  //global.environment->functionEvaluations -= 1;
	  //populationFitnessAverage += (child->fitness - entity->fitness) > 0;
	  populationFitnessAverage += child->fitness;
	  populationGestationTimeAverage += child->gestationTime;
	  //longWindowFitnessAverage += (child->fitness - entity->fitness) > 0;
	  longWindowFitnessAverage += child->fitness;

	  // # function evaluations is only ever updated inside this block. Do all time-related things here.
	  // Output population average fitness and average gestation time
	  if ((global.environment->functionEvaluations) % (args.width*args.height) == 0)
	  {
	    dataFile1 << global.environment->functionEvaluations << "\t" << populationFitnessAverage/(args.width*args.height) << "\t" << populationGestationTimeAverage/(float)(args.width*args.height) << endl;
	    populationFitnessAverage = 0.0;
	    populationGestationTimeAverage = 0;
	  }
	  // Output long window average fitness
	  if ((global.environment->functionEvaluations) % args.averagingWindowLength == 0)
	  {
	    dataFile2 << global.environment->functionEvaluations - args.averagingWindowLength/2 << "\t" << longWindowFitnessAverage/(float)args.averagingWindowLength << endl;
	    longWindowFitnessAverage = 0.0;
	  }
	  if ((global.environment->functionEvaluations) % 20000 == 0)
	  {
	    //if (global.grids[global.gridIndex][0][0])
	    //  global.grids[global.gridIndex][0][0]->printCount=10;
	  }

	  // Every so often, add new (orphan) entities to one line of the grid
	  if (global.environment->functionEvaluations%(args.spawnRate*args.width*args.height)==args.spawnRate*args.width*args.height-1)
	    placeNewOrganisms = 1;

	}

      } // end of if entity exists check
    } // end of cell update
  } // end of row update

  if (placeNewOrganisms)
  {
    newOrganisms(args, global);
    //debugPrint(args, global);
  }

}



// Clean up memory
void cleanUp(config &args, globalVars &global)
{
  for(int i=0; i<args.width; i++)
  {
    for(int j=0; j<args.height; j++)
    {
      if (global.grids[0][i][j] == global.grids[1][i][j])
	global.grids[1][i][j] = NULL;
      for(int g=0; g<2; g++)
      {
	if (global.grids[g][i][j])
	{
	  deleteOrganism(args, global.grids[g][i][j]);
	}
      }
    }
  }

  deleteEnvironment(args, global.environment);
}


int main(int argc, char **argv)
{
  config args;
  globalVars global;

  // Parse arguments and initialize grid
  if ( initialize(argc, argv, args, global) )
    return 0;

  ofstream dataFile1, dataFile2;
  dataFile1.open((args.resultsBaseDir + args.resultsDir + "time1.dat").c_str());
  dataFile2.open((args.resultsBaseDir + args.resultsDir + "time2.dat").c_str());
  ofstream reproducerFile;
  reproducerFile.open((args.resultsBaseDir + args.resultsDir + "reproducers.dat").c_str());

  // Enter the main loop
  while(global.environment->functionEvaluations < args.totalFunctionEvaluations)
  {
    loop(args, global, dataFile1, dataFile2, reproducerFile);
  }

  dataFile1.close();
  dataFile2.close();
  reproducerFile.close();

  // Free allocated memory
  cleanUp(args, global);
  return 0;
}
