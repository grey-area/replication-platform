#include <iostream>
#include <algorithm>
#include <time.h>
#include <sys/stat.h>

#include <sys/types.h> // These 2 are for getpid()
#include <unistd.h>

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
#include "BaseOrganism.h"
#include "BaseEnvironment.h"
#include "module_handler.h"

float populationMetabolismAverage = 0.0;
int populationGestationTimeAverage = 0;

// Put some random, new (orphan) organisms on the grid. Periodically called. (TODO How frequently? Use spawn rate parameter?)
void newOrganisms(config &args, globalVars &global)
{
  for(int j=0;j<args.height; j++)
  {
    // If there's already a new organism there, delete it
    if ( global.grids[1-global.gridIndex][0][j] and global.grids[1-global.gridIndex][0][j] != global.grids[global.gridIndex][0][j])
    {
      // Add the organism to the DB
      global.grids[1-global.gridIndex][0][j]->addToDB(args, global);
      deleteOrganism(args, global.grids[1-global.gridIndex][0][j]);
    }
    // Ask the model_handler for a new organism
    global.grids[1-global.gridIndex][0][j] = newOrganism(args, global); 
    global.grids[1-global.gridIndex][0][j]->initializeOrphan(args);
  }

  //cout << "Placing new organisms" << endl;
  //debugPrint(args, global);
}

// Place the child on the grid
void placeChild(config &args, globalVars &global, BaseOrganism *child, int i, int j, int &x, int &y)
{
  // Put the child in a random cell neighbouring the parent (in the other grid)
  x = mod((i+rand()%3-1), args.width);
  y = mod((j +rand()%3-1), args.height);

  // If the cell is already occupied by a new organism, delete it
  if (global.grids[1-global.gridIndex][x][y] and global.grids[1-global.gridIndex][x][y] != global.grids[global.gridIndex][x][y])
  {
    global.grids[1-global.gridIndex][x][y]->addToDB(args, global);
    deleteOrganism(args, global.grids[1-global.gridIndex][x][y]);
  }

  global.grids[1-global.gridIndex][x][y] = child;

  //cout << "Placing child" << endl;
  //debugPrint(args, global);
}

// Initialization
int initialize(int argc, char **argv, config &args, globalVars &global)
{
  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.organismModuleName is the name of the developmental mechanism we'll use
  if ( parseArguments(argc, argv, args) )
    return 1;

  global.generation = 0;

  // Set the seed. Use time + process ID
  args.pid = getpid();
  if (args.seed==-1)
    args.seed = time(NULL) + args.pid;
  srand(args.seed);

  // Make the directory in which the results will go
  setResultsDir(args);
  _mkdir((args.resultsBaseDir + args.resultsDir).c_str());

  // Open results DB. Add table for recording organisms
  sqlite3_open(args.dbString.c_str(), &(global.db));
  string query = "CREATE TABLE Organisms (ID INT, ParentID INT, X INT, Y INT, StartGeneration INT, EndGeneration INT, Metabolism INT, Fitness INT, Genome TEXT, Solution TEXT, Other TEXT);";
  sqlite3_exec(global.db, query.c_str(), NULL, NULL, &(global.sqlErrorMsg));

  sqlite3_exec(global.db, "BEGIN TRANSACTION", NULL, NULL, &(global.sqlErrorMsg));

  // Write the current options to a config file in the results directory
  writeConfigFile(args);

  BaseOrganism::setArgs(args);

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
  global.nextID = 0;

  // Ask the model_handler for a new environment object
  global.environment = newEnvironment(args);

  // Put some new (orphan) entities on the grid
  newOrganisms(args, global);

  // Open file for recording population statistics
  global.dataFile.open((args.resultsBaseDir + args.resultsDir + "population_stats.dat").c_str());

  return 0;
}


// Main loop
int loop(config &args, globalVars &global)
{
  global.gridIndex = 1 - global.gridIndex;
  global.generation++;

  int headCount = 0;
  int placeNewOrganisms = 0;

  // If metabolisms are periodically updated, update them
  global.environment->updateMetabolisms(args, global);

  // Call update on each organism periodically
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
	      global.grids[1-global.gridIndex][i][j]->addToDB(args, global);
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

	// TODO change selection mechanism
	// Tournament selection between this entity and one of its neighbours. Selection determines who gets executed next
	int i2 = mod(i + rand()%3 - 1, args.width);
	int j2;
	if (i2==i)
	  j2 = mod(j + (rand()%2==0 ? 1 : -1), args.height);
	else
	  j2 = mod(j + rand()%3 - 1, args.height);
	int ie = i;
	int je = j;
	if (global.grids[global.gridIndex][i2][j2] and global.grids[global.gridIndex][i2][j2]->metabolism > global.grids[global.gridIndex][i][j]->metabolism)
	{
	  ie = i2;
	  je = j2;
	}

	//cout << "update " << ie << ", " << je << endl << endl;

	BaseOrganism *entity = global.grids[global.gridIndex][ie][je];

	// Update the entity. See BaseOrganism.cpp for update cycle. Most time will be spent calling the decode function
	// which is implemented by a subclass of BaseOrganism
	entity->update(args, global);

	// If the entity has a non-embryonic child (has just reproduced)
	if (entity->child and (entity->child)->state != EMBRYO)
	{
	  BaseOrganism *child = entity->child;
	  entity->child = NULL;

	  int x,y;
	  // Place the child in the neighbourhood of the parent.
	  placeChild(args, global, child, ie, je, x, y);
	  
	  // Send the child's body specification to the task environment, setting its metabolism.
	  global.environment->interpretBody(args, global, x, y);

	  // TODO added this to discount drops due to function change, and added below to record probability of beneficial mutation
	  //global.environment->interpretBody(args, ie, je);
	  //global.environment->functionEvaluations -= 1;
	  //populationMetabolismAverage += (child->metabolism - entity->metabolism) > 0;
	  populationMetabolismAverage += child->metabolism;
	  populationGestationTimeAverage += child->gestationTime;

	  // # function evaluations is only ever updated inside this block. Do all time-related things here.
	  // Output population average metabolism and average gestation time
	  if ((global.environment->functionEvaluations) % (args.width*args.height) == 0)
	  {
	    global.dataFile << global.generation << "\t" << populationMetabolismAverage/(args.width*args.height) << "\t" << populationGestationTimeAverage/(float)(args.width*args.height) << endl;
	    populationMetabolismAverage = 0.0;
	    populationGestationTimeAverage = 0;
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
  global.dataFile.close();

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
	  global.grids[g][i][j]->addToDB(args, global);
	  deleteOrganism(args, global.grids[g][i][j]);
	}
      }
    }
  }

  sqlite3_exec(global.db, "END TRANSACTION", NULL, NULL, &(global.sqlErrorMsg));
  sqlite3_close(global.db);

  deleteEnvironment(args, global.environment);
}


int main(int argc, char **argv)
{
  config args;
  globalVars global;

  // Parse arguments and initialize grid
  if ( initialize(argc, argv, args, global) )
    return 0;

  // Enter the main loop
  while(global.environment->functionEvaluations < args.totalFunctionEvaluations)
  {
    loop(args, global);
  }

  // Free allocated memory
  cleanUp(args, global);
  return 0;
}
