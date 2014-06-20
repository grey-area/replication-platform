using namespace std;

#include <iostream>
#include "BaseOrganism.h"
#include "module_handler.h"
#include <algorithm>
#include <set>
#include <sstream>

// ** = override
// *  = optionally override

string BaseOrganism::organismModuleName;
int BaseOrganism::alphabetSize;
int BaseOrganism::initialGenomeSize;
int BaseOrganism::minGenomeSize;
int BaseOrganism::maxGenomeSize;

void BaseOrganism::setArgs(config &args)
{
  organismModuleName = args.organismModuleName;
  if (args.organismArgs.count("alphabet"))
    alphabetSize = atoi(args.organismArgs["alphabet"].c_str());
  else
    alphabetSize = 10;
  if (args.organismArgs.count("initial-genome-size"))
    initialGenomeSize = atoi(args.organismArgs["initial-genome-size"].c_str());
  else
    initialGenomeSize = 10;
  if (args.organismArgs.count("min-genome-size"))
    minGenomeSize = atoi(args.organismArgs["min-genome-size"].c_str());
  else
    minGenomeSize = 1;
  if (args.organismArgs.count("max-genome-size"))
    maxGenomeSize = atoi(args.organismArgs["max-genome-size"].c_str());
  else
    maxGenomeSize = 100;
}

BaseOrganism::BaseOrganism(config &args, globalVars &global)
{
  id = global.nextID;
  global.nextID++; 

  parentID = 0;

  // The default state of a new organism
  state   = EMBRYO;
  generationOfBirth = global.generation;
  age     = 0;
  gestationTime = 0;

  metabolism = 0.0;
  child = NULL;
}

BaseOrganism::~BaseOrganism()
{
  // If we've alloc'd a child who hasn't been born, free it
  config args;
  args.organismModuleName = organismModuleName;
  if (child and (child->state)==EMBRYO)
  {
    deleteOrganism(args, child);
  }
}


// *
void BaseOrganism::printGenome(ofstream &stream)
{
  stream << "Genome:" << endl;
  for (vector<unsigned short>::iterator it = genome.begin() ; it != genome.end(); ++it)
    stream << *it << " ";
  stream << endl;
}

// *
void BaseOrganism::printSomaSpecification(ofstream &stream)
{
  stream << "Body specification:" << endl;
  for (vector<unsigned short>::iterator it = somaSpecification.begin(); it != somaSpecification.end(); ++it)
    stream << *it << " ";
  stream << endl;
}

// **
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
void BaseOrganism::printDevelopmentProcess(ofstream &stream)
{
}


void BaseOrganism::print(ofstream &stream)
{
  stream << "ID: " << id << " Parent ID: " << parentID << endl;
  stream << "Gestation time: " << gestationTime << endl;
  stream << "Metabolism: " << metabolism << endl;
  printGenome(stream);
  printSomaSpecification(stream);
  printDevelopmentProcess(stream);
  stream << endl << endl;
}


// *
// New random genome for orphan organism.
void BaseOrganism::initializeOrphanGenome(config &args)
{
  genome.resize(initialGenomeSize);
  int i;
  for (i=0; i<initialGenomeSize; i++)
    genome[i] = rand()%alphabetSize;
}

// **
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
void BaseOrganism::initializeOrphanDevelopmentProcess(config &args)
{
}

// Initialize orphan organism
void BaseOrganism::initializeOrphan(config &args)
{
  state = START;
  initializeOrphanGenome(args);
  initializeOrphanDevelopmentProcess(args);
}


// **
// Called at the start of each development cycle.
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
void BaseOrganism::initializeDevelopmentProcess(config &args)
{
}

// Return a copy of the genome. Used for copying genome to child.
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
vector<unsigned short> BaseOrganism::copyGenome(config &args)
{
  vector<unsigned short> genomeCopy (genome);

  return genomeCopy;
}

// *
// Mutate the genome of the organism in place
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
void BaseOrganism::mutateGenome(config &args, vector<unsigned short> &genome1)
{
  // Insert with probability 0.5
  if (rand()%2==0 and genome1.size() < maxGenomeSize)
    genome1.insert(genome1.begin()+rand()%(genome1.size()), rand()%alphabetSize);
  // Delete with probability 0.5
  if (rand()%2==0 and genome1.size() > minGenomeSize)
    genome1.erase(genome1.begin()+rand()%(genome1.size()));
  // Change with probability 0.5
  if (rand()%2==0)
    genome1.at(rand()%(genome1.size())) = rand()%alphabetSize;  
}

void BaseOrganism::addToDB(config &args, globalVars &global)
{
  // Only record organisms if record level > 0
  if (args.recordLevel > 0)
  {
    
    string query = "INSERT INTO Organisms (ID, ParentID, X, Y, StartGeneration, EndGeneration, Metabolism, Fitness, Genome, Solution, Other) VALUES (";
    string result;
    stringstream convert;
    convert << id << "," << parentID << ",0,0," << generationOfBirth << "," << global.generation << "," << metabolism << ",0,";

    
    if (args.recordLevel == 1)
      convert << "'','','');";
    else // TODO only record genome, solution, and organism configuration if record level > 1
      convert << "'test','test','test');";

    query = query + convert.str();

    sqlite3_exec(global.db, query.c_str(), NULL, NULL, &(global.sqlErrorMsg));
  }
}


// **
// Iteratively called during the decode cycle.
/*! \brief Brief description.
 *         Brief description continued.
 *
 *  Detailed description starts here.
 */
void BaseOrganism::updateDevelopment(config &args)
{
  // The default behaviour is that the genome and the `phenotype' are the same thing
  copy(child->genome.begin(), child->genome.end(), back_inserter(child->somaSpecification));
  state = REPRODUCED;
}


void BaseOrganism::update(config &args, globalVars &global)
{
  switch(state)
  {
    case START:

      // Create new child
      child = newOrganism(args, global);
      child->parentID = id;
      gestationTime = 0;

      // Copy genome to child
      child->genome = copyGenome(args);
      mutateGenome(args, child->genome);

      state = GESTATION;
      initializeDevelopmentProcess(args);
      break;

    case GESTATION:

      // Subclass implements this. When it's finished development, we expect it to
      // change state to REPRODUCED, and to have given the child a development function
      // and body specification
      child->gestationTime++;
      updateDevelopment(args);

      break;

    case REPRODUCED:

      child->metabolism = metabolism;
      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle

      break;

    default: break;
  }

}
