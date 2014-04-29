using namespace std;

#include <iostream>
#include "BaseDevMechanism.h"
#include "module_handler.h"
#include <algorithm>
#include <set>

// ** = override
// *  = optionally override

string BaseDevMechanism::devName;
int BaseDevMechanism::alphabetSize;
int BaseDevMechanism::initialDataSize;
int BaseDevMechanism::minDataSize;
int BaseDevMechanism::maxDataSize;

void BaseDevMechanism::setArgs(config &args)
{
  devName = args.developmentMechanism;
  if (args.devArgs.count("alphabet"))
    alphabetSize = atoi(args.devArgs["alphabet"].c_str());
  else
    alphabetSize = 10;
  if (args.devArgs.count("initial-data-size"))
    initialDataSize = atoi(args.devArgs["initial-data-size"].c_str());
  else
    initialDataSize = 10;
  if (args.devArgs.count("min-data-size"))
    minDataSize = atoi(args.devArgs["min-data-size"].c_str());
  else
    minDataSize = 1;
  if (args.devArgs.count("max-data-size"))
    maxDataSize = atoi(args.devArgs["max-data-size"].c_str());
  else
    maxDataSize = 100;
}

BaseDevMechanism::BaseDevMechanism(config &args)
{
  id = rand()%1000000;
  parentID = 0;

  // The default state of a new organism
  state   = EMBRYO;
  age     = 0;
  gestationTime = 0;

  fitness = 0.0;
  score   = 0.0;
  child = NULL;
  printCount = 0;
}

BaseDevMechanism::~BaseDevMechanism()
{
  // If we've alloc'd a child who hasn't been born, free it
  config args;
  args.developmentMechanism = devName;
  if (child and (child->state)==EMBRYO)
    deleteOrganism(args, child);
}


// *
void BaseDevMechanism::printData(ofstream &stream)
{
  stream << "Data:" << endl;
  for (vector<unsigned short>::iterator it = data.begin() ; it != data.end(); ++it)
    stream << *it << " ";
  stream << endl;
}

// *
void BaseDevMechanism::printBodySpecification(ofstream &stream)
{
  stream << "Body specification:" << endl;
  for (vector<unsigned short>::iterator it = bodySpecification.begin(); it != bodySpecification.end(); ++it)
    stream << *it << " ";
  stream << endl;
}

// **
void BaseDevMechanism::printDecoder(ofstream &stream)
{
}


void BaseDevMechanism::print(ofstream &stream)
{
  stream << "ID: " << id << " Parent ID: " << parentID << endl;
  stream << "Print count: " << printCount << endl;
  stream << "Gestation time: " << gestationTime << endl;
  printData(stream);
  printBodySpecification(stream);
  printDecoder(stream);
  stream << endl << endl;
}


// *
// New random data for orphan organism.
void BaseDevMechanism::initializeOrphanData(config &args)
{
  data.resize(initialDataSize);
  int i;
  for (i=0; i<initialDataSize; i++)
    data[i] = rand()%alphabetSize;
}

// **
void BaseDevMechanism::initializeOrphanDecoder(config &args)
{
}

// Initialize orphan organism
void BaseDevMechanism::initializeOrphan(config &args)
{
  state = START;
  initializeOrphanData(args);
  initializeOrphanDecoder(args);
}


// **
// Called at the start of each decoding cycle.
void BaseDevMechanism::initializeDecoding(config &args)
{
}

// Return a copy of the data. Used for copying data to child.
vector<unsigned short> BaseDevMechanism::copyData(config &args)
{
  vector<unsigned short> dataCopy (data);

  return dataCopy;
}

// *
// Mutate the data of the organism in place
void BaseDevMechanism::mutateData(config &args, vector<unsigned short> &data1)
{
  // Insert with probability 0.5
  if (rand()%2==0 and data1.size() < maxDataSize)
    data1.insert(data1.begin()+rand()%(data1.size()), rand()%alphabetSize);
  // Delete with probability 0.5
  if (rand()%2==0 and data1.size() > minDataSize)
    data1.erase(data1.begin()+rand()%(data1.size()));
  // Change with probability 0.5
  if (rand()%2==0)
    data1.at(rand()%(data1.size())) = rand()%alphabetSize;  
}


// **
// Iteratively called during the decode cycle.
void BaseDevMechanism::decode(config &args)
{
  // The default behaviour is that the data and the `phenotype' are the same thing
  copy(child->data.begin(), child->data.end(), back_inserter(child->bodySpecification));
  state = REPRODUCED;
}


// Iteratively called by the controller. What we do here depends on our state, which is mostly controlled by this base class.
// Roughly: 1) Create child and copy data to it. 2) The subclass decodes the child's data and interprets the result as a decoder and a body specification. It gives both to the child. 3) Give child same fitness level as us and both go to state start.
void BaseDevMechanism::update(config &args)
{
  age++;

  switch(state)
  {
    case START:

      // Create new child
      child = newOrganism(args);
      child->parentID = id;
      gestationTime = 0;

      // Copy data to child
      child->data = copyData(args);
      mutateData(args, child->data);

      state = DECODING;
      initializeDecoding(args);
      break;

    case DECODING:

      // Subclass implements this. When it's finished decoding, we expect it to
      // change state to REPRODUCED, and to have given the child a decoding function
      // and body specification
      child->gestationTime++;
      decode(args);

      break;

    case REPRODUCED:
      
      child->fitness = fitness;
      child->score   = score;
      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle

      if (printCount > 0)
      {
	child->printCount = printCount-1;
	printCount = 0;
      }
      
      break;

    default: break;
  }

}
