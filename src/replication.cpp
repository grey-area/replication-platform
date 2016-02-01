#include <iostream>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <sys/stat.h>

// These 2 are for getpid(), used so that each process has a different seed
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#include "config.h"
#include "misc.h"
#include "BaseOrganism.h"
#include "BaseEnvironment.h"
#include "module_handler.h"


// Add an organism to the database.
// TODO restructure DB. Include main results in it.
void addOrganismToDatabase(config &args, globalVars &global, int frame, int i, int j)
{
  // Only record organisms if record level > 0
  if (args.recordLevel > 0)
  {
    BaseOrganism *o = global.grids[frame][i][j];
    
    string query = "INSERT INTO Organisms (ID, ParentID, X, Y, StartGeneration, EndGeneration, Metabolism, Fitness, Species, Genome, Solution, Other) VALUES (";
    string result;
    stringstream convert;
    convert << o->id << "," << o->parentID << "," << i << "," << j << "," << o->generationOfBirth << "," << global.generation << "," << o->metabolism << ",0," << o->species << ",";

    if (args.recordLevel == 1)
      convert << "'','','');";
    else // TODO only record genome, solution, and organism configuration if record level > 1
      convert << "'test','test','test');";

    query = query + convert.str();

    sqlite3_exec(global.db, query.c_str(), NULL, NULL, &(global.sqlErrorMsg));
  }
}


// Record and organism to the database and then have the model handler delete it.
void recordAndDeleteOrganism(config &args, globalVars &global, int frame, int i, int j)
{
  addOrganismToDatabase(args, global, frame, i, j);
  deleteOrganism(args, global.grids[frame][i][j]);
}

// Is this space occupied by a new organism (one that exists only in the next frame, and
// not the current one)? If so, explicitly delete it because it won't be cleaned up later.
// Note: gridIndex is the current frame, 1-gridIndex is the next frame.
int deleteIfNewlyOccupied(config &args, globalVars &global, int x, int y)
{
  if (global.grids[1-global.gridIndex][x][y] and global.grids[1-global.gridIndex][x][y] != global.grids[global.gridIndex][x][y])
    recordAndDeleteOrganism(args, global, 1-global.gridIndex, x, y);
}


// Put some random, new (orphan) organisms on the grid on column 0. Periodically called.
void newOrphanOrganisms(config &args, globalVars &global)
{
  for (int j=0; j<args.height; j++)
  {
    // If there's already a new organism there, explicitly delete it.
    deleteIfNewlyOccupied(args, global, 0, j);

    // Ask the model_handler for a new orphan organism
    global.grids[1-global.gridIndex][0][j] = newOrganism(args, global); 
    // TODO should this be handled elsewhere?
    global.grids[1-global.gridIndex][0][j]->initializeOrphan(args);

    // TODO organism shouldn't store position?
    global.grids[1-global.gridIndex][0][j]->x = 0;
    global.grids[1-global.gridIndex][0][j]->y = j;
  }

}


// Put the child in a random cell neighbouring the parent (in the next frame)
void placeChild(config &args, globalVars &global, BaseOrganism *child, int i, int j, int &x, int &y)
{
  x = mod((i + rand()%3 - 1), args.width);
  y = mod((j + rand()%3 - 1), args.height);

  // If there's already a new organism there, explicitly delete it.
  deleteIfNewlyOccupied(args, global, x, y);

  global.grids[1-global.gridIndex][x][y] = child;

  // TODO organism shouldn't store position?
  child->x = x;
  child->y = y;
}


// The time and process ID uniquely identify this run.
// This will be used as part of the output directory.
// If we didn't set a seed from the config file, use this as the seed.
void initializeSeed(config &args)
{
  args.pid = getpid();
  if (args.seed==-1) 
    args.seed = time(NULL) + args.pid;
  srand(args.seed);
}


// Make the directory in which the results will go
void initializeDirectory(config &args, globalVars &global)
{
  setResultsDir(args); // in config.cpp
  _mkdir((args.resultsBaseDir + args.resultsDir).c_str());

  // Write the current options to a config file in the results directory
  writeConfigFile(args);

  // Open file for recording population statistics
  global.dataFile.open((args.resultsBaseDir + args.resultsDir + "population_stats.dat").c_str());
}


// Open results DB. Add table for recording organisms.
void initializeResultsDatabase(config &args, globalVars &global)
{
  sqlite3_open(args.dbString.c_str(), &(global.db));
  string query = "CREATE TABLE Organisms (ID INT, ParentID INT, X INT, Y INT, StartGeneration INT, EndGeneration INT, Metabolism INT, Fitness INT, Species INT, Genome TEXT, Solution TEXT, Other TEXT);";
  sqlite3_exec(global.db, query.c_str(), NULL, NULL, &(global.sqlErrorMsg));

  sqlite3_exec(global.db, "BEGIN TRANSACTION", NULL, NULL, &(global.sqlErrorMsg));
}


// Close the results DB. And index for searching by generation.
void closeResultsDatabase(globalVars &global)
{
  sqlite3_exec(global.db, "END TRANSACTION", NULL, NULL, &(global.sqlErrorMsg));

  string query = "CREATE INDEX generation_index ON Organisms (StartGeneration, EndGeneration);";
  sqlite3_exec(global.db, query.c_str(), NULL, NULL, &(global.sqlErrorMsg));

  sqlite3_close(global.db);
}


// Initialize a 2D grid of pointers to (NULL) organisms
// Then put some orphans on the grid
void initializeGrid(config &args, globalVars &global)
{
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

  // Put some new orphans on the grid
  newOrphanOrganisms(args, global);
}


// Initialize the grid, seed, 
void initialize(config &args, globalVars &global)
{
  global.generation = 0;

  initializeSeed(args);

  initializeDirectory(args, global);

  initializeResultsDatabase(args, global);

  BaseOrganism::setArgs(args); // Pass arguments on to the organism.

  initializeGrid(args, global);

  // Ask the model_handler for a new environment object of the proper type.
  global.environment = newEnvironment(args);
}


// We've just switched frame, so the other frame has to be made to equal this one.
// Clean up replaced organism, if necessary.
void updateSecondFrame(config &args, globalVars &global, int i, int j)
{
  if (global.grids[global.gridIndex][i][j] != global.grids[1-global.gridIndex][i][j])
  {
    // If there is something in the old grid and it's not new:
    if ( global.grids[1-global.gridIndex][i][j] )
    {
      if ( global.grids[1-global.gridIndex][i][j]->age > 0 )
      {
	// Delete the old.
	recordAndDeleteOrganism(args, global, 1-global.gridIndex, i, j);

	// Replace it with the new.
	global.grids[1-global.gridIndex][i][j] = global.grids[global.gridIndex][i][j];
      }
    }
    else // if there isn't something in old grid 
      global.grids[1-global.gridIndex][i][j] = global.grids[global.gridIndex][i][j];
  }
}


// Tournament selection between this entity and one of its neighbours.
BaseOrganism* selectOrganism(config &args, globalVars &global, int &i, int &j)
{
  // Select a neighbour.
  int i2 = mod(i + rand()%3 - 1, args.width);
  int j2 = mod(j + (i2==i ? (rand()%2==0 ? 1 : -1) : rand()%3 - 1), args.height);

  // If the competitor exists and is better than the current organism, give up the update cycle to it.
  // TODO: for environments where fitness/score is determined by competition, I might want to do that here
  if (global.grids[global.gridIndex][i2][j2] and global.grids[global.gridIndex][i2][j2]->metabolism > global.grids[global.gridIndex][i][j]->metabolism)
  {
    i = i2;
    j = j2;
  }

  return global.grids[global.gridIndex][i][j];
}


// Unlink the parent and child. Place the child in the grid.
// Have the environment interpret the child to produce a fitness/score.
// TODO: think about this last part, and the possible timings (e.g., what about
// when fitness/score is determined by competition?)
void handleNewChild(config &args, globalVars &global, int i, int j)
{
  BaseOrganism *parent = global.grids[global.gridIndex][i][j];
  BaseOrganism *child = parent->child;
  parent->child = NULL;

  int x,y; // Place the child in the neighbourhood of the parent.
  placeChild(args, global, child, i, j, x, y);
	  
  // Send the child's body specification to the task environment, setting its metabolism.
  global.environment->interpretBody(args, global, x, y);

  // TODO these lines were recording average 'metabolism' and gestation time for outputing
  //populationMetabolismAverage += child->metabolism;
  //populationGestationTimeAverage += child->gestationTime;

  // One 'generation'. Handle time related output here.
  // TODO: change metabolism, fitness, etc. Output to DB instead of file?
  // TODO: If I change when function evaluation happens, I might have to move this
  if ((global.environment->functionEvaluations) % (args.width*args.height) == 0)
  {
    //global.dataFile << global.generation << "\t" << populationMetabolismAverage/(args.width*args.height) << "\t" << populationGestationTimeAverage/(float)(args.width*args.height) << endl;
    //populationMetabolismAverage = 0.0;
    //populationGestationTimeAverage = 0;
  }
}


void iterateOrganism(config &args, globalVars &global, int i, int j)
{
  // Clean up replaced organism on the other frame, if necessary.
  updateSecondFrame(args, global, i, j);

  global.grids[global.gridIndex][i][j]->age++;

  // Select between this organism and one of its neighbours. Note, this function modifies i, j as necessary.
  BaseOrganism *victor = selectOrganism(args, global, i, j);

  // Update the entity. See BaseOrganism.cpp for update cycle. Most time will be spent calling the decode function
  // which is implemented by a subclass of BaseOrganism.
  // TODO add a config argument: number of updates to perform per iteration
  victor->update(args, global);

  // If the entity has a non-embryonic child (has just reproduced in the above update):
  if (victor->child and (victor->child)->state != EMBRYO)
    handleNewChild(args, global, i, j);
}


// Iterate through the population
int iterate(config &args, globalVars &global)
{
  global.gridIndex = 1 - global.gridIndex; // Switch frame
  global.generation++;

  // If metabolisms are periodically updated, update them
  // TODO: Rethink the way that 'metabolism' is updated. Here I had invisaged that they might be periodically updated. I think the distinction has to be between organisms whose score is set once upon creation, organisms that keep receiving inputs to do something with, and organisms that interact with each other.
  global.environment->updateMetabolisms(args, global);

  bool spawnNewOrphans = false;

  // For each cell
  for(int i=0; i<args.width; i++) 
  {
    for(int j=0; j<args.height; j++)
    {
      if (global.grids[global.gridIndex][i][j]) // If there is an entity there
      {
	iterateOrganism(args, global, i, j);
	
	// If the number of function evaluations is spawnRate*population size, spawn some new orphans.
	if (global.environment->functionEvaluations%(args.spawnRate*args.width*args.height)==args.spawnRate*args.width*args.height-1)
	  spawnNewOrphans = true;
      }
    } 
  } 

  // Periodically place new orphans in the grid
  if (spawnNewOrphans)
    newOrphanOrganisms(args, global);

}


// Clean up memory
void cleanUpMemory(config &args, globalVars &global)
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
	  // TODO couldn't this add the same organism twice?
	  //global.grids[g][i][j]->addOrganismToDatabase(args, global);
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

  // Parse command line arguments. Put result in args struct (see arguments.h).
  if ( parseArguments(argc, argv, args) )
    return 1;

  // Initialize the grid, results directory, results DB, the environment.
  initialize(args, global);


  // TODO temp serialization
  //ofstream dump;
  //dump.open("temp.txt");
  //serialize(dump, global);
  //dump.close();
  //exit(0);
  
  // The main loop
  while(global.environment->functionEvaluations < args.totalFunctionEvaluations)
  {
    iterate(args, global);
  }

  // Free allocated memory
  cleanUpMemory(args, global);

  // Close the results database
  closeResultsDatabase(global);

  return 0;
}
