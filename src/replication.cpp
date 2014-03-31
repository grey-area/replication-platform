#include <iostream>
#include <algorithm>
#include <time.h>
#include <fstream>

using namespace std;

#include "config.h"
#include "misc.h"
#include "BaseReplicator.h"
#include "BaseEnvironment.h"
#include "modelHandler.h"

#include <sys/stat.h>

vector <vector <BaseReplicator* > > grid;
BaseEnvironment *environment;

float window1Average = 0.0;
float window2Average = 0.0;
float wholeResult = 0.0;
float decilesResult [10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float quartilesResult [4] = {0.0,0.0,0.0,0.0};

// Put some brand new (orphan) entities on the grid. Periodically called.
void newEntities(config &args)
{
  //for(int j=0;j<1; j++)
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
  // Parse command line arguments, put result in args struct (see arguments.h)
  // args.model is the name of the replication model we'll use
  if ( parseArguments(argc, argv, args) )
    return 1;

  if (args.seed==-1)
    args.seed = time(NULL);
  srand(args.seed);

  setConfigDir(args);
  // Make the directory in which the results will go
  _mkdir((args.resultsBaseDir + args.resultsConfigDir).c_str());
  // Wait to here to write config file, because we want to write the particular seed used
  writeConfigFile(args);

  BaseReplicator::setArgs(args);

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
int loop(config &args, int t, int &lastT, ofstream &dataFile1, ofstream &dataFile2, ofstream &reproducerFile)
{
  int headCount = 0;
  float totalScore = 0.0;
  float maxScore = 0.0;
  int totalGestationTime = 0;

  // Every so often, add new (orphan) entities to one line of the grid
  if (t%100==99)
    newEntities(args);

  // If fitnesses are periodically updated, update them
  // TODO: wholeResult, decilesResult and quartilesResult don't count these updated fitnesses
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
	
	BaseReplicator *entity = grid[ie][je];
	// TODO: what do I want to be printed when debugging?
	//if (args.debug)
	//  cout << "Updating " << ip << "," << j << endl;

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
	  placeChild(args, child, ie, je, x, y);
	  environment->interpretBody(args, x, y, t);

	  window1Average += child->fitness;
	  window2Average += child->fitness;
	  // TODO temp
	  if ((environment->functionEvaluations) % (args.width*args.height) == 0)
	  {
	    dataFile1 << environment->functionEvaluations << "\t" << window1Average/(args.width*args.height) << endl;
	    window1Average = 0.0;
	  }
	  if ((environment->functionEvaluations) % 100000 == 0)
	  {
	    dataFile2 << environment->functionEvaluations - 50000 << "\t" << window2Average/100000.0 << endl;
	    window2Average = 0.0;
	  }

	  wholeResult += child->fitness;
	  decilesResult[(environment->functionEvaluations * 10) / args.simulationTime] += child->fitness;
	  quartilesResult[(environment->functionEvaluations * 4) / args.simulationTime] +=  child->fitness;


	}

      } // end of if entity exists check
    } // end of cell update
  } // end of row update

  // TODO what do we want to print/output?
  // TODO replaced with temp above
  /*  if(t-lastT1 > 2000) 
  {
    dataFile << environment->functionEvaluations << "\t" << maxScore << "\t" << totalScore/(float)headCount << "\t" << headCount << "\t" << totalGestationTime/(float)headCount << endl;
    lastT1 = t;
  }
  */

  if(t-lastT > 20000)
  {
    if (grid[0][0])
    {
      reproducerFile << "Function evaluations:" << environment->functionEvaluations << endl;
      grid[0][0]->print(reproducerFile);
    }
    lastT = t;
  }

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

  ofstream dataFile1, dataFile2;
  dataFile1.open((args.resultsBaseDir + args.resultsConfigDir + "time1.dat").c_str());
  dataFile2.open((args.resultsBaseDir + args.resultsConfigDir + "time2.dat").c_str());
  ofstream reproducerFile;
  reproducerFile.open((args.resultsBaseDir + args.resultsConfigDir + "reproducers.dat").c_str());

  // Enter the main loop
  //for(int t=0; t<args.simulationTime; t++)
  int lastFunctionEvaluations = 0;
  while(environment->functionEvaluations < args.simulationTime)
  {
    loop(args, environment->functionEvaluations, lastFunctionEvaluations, dataFile1, dataFile2, reproducerFile);
  }

  dataFile1.close();
  dataFile2.close();
  reproducerFile.close();


  int i;
  dataFile1.open((args.resultsBaseDir + args.resultsConfigDir + "data.dat").c_str());
  dataFile1 << "avgFitness = " << wholeResult/args.simulationTime << endl;
  for(i=0; i<10; ++i)
    dataFile1 << "avgFitnessDec" << i << " = " << decilesResult[i] / (args.simulationTime/10) << endl;
  for(i=0; i<4; ++i)
    dataFile1 << "avgFitnessQuart" << i << " = " << quartilesResult[i] / (args.simulationTime/4) << endl;
  dataFile1 << "avgFitnessDec9Minus0 = " << (decilesResult[9] - decilesResult[0]) / (args.simulationTime/10) << endl;
  dataFile1 << "avgFitnessQuart3Minus0 = " << (quartilesResult[3] - quartilesResult[0]) / (args.simulationTime/4) << endl;
  dataFile1.close();

  // Free allocated memory
  cleanUp(args);
  return 0;
}
