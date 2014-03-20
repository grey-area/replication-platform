using namespace std;

#include <iostream>
#include "BaseReplicator.h"
#include "modelHandler.h"
#include <algorithm>
#include <set>

string BaseReplicator::modelName;
int BaseReplicator::alphabetSize;
int BaseReplicator::initialDataSize;
int BaseReplicator::minDataSize;
int BaseReplicator::maxDataSize;

void BaseReplicator::setArgs(config &args)
{
  modelName = args.model;
  if (args.modelConfig.count("alphabet"))
    alphabetSize = atoi(args.modelConfig["alphabet"].c_str());
  else
    alphabetSize = 31;
  if (args.modelConfig.count("initial-data-size"))
    initialDataSize = atoi(args.modelConfig["initial-data-size"].c_str());
  else
    initialDataSize = 10;
  if (args.modelConfig.count("min-data-size"))
    minDataSize = atoi(args.modelConfig["min-data-size"].c_str());
  else
    minDataSize = 1;
  if (args.modelConfig.count("max-data-size"))
    maxDataSize = atoi(args.modelConfig["max-data-size"].c_str());
  else
    maxDataSize = 100;
}

BaseReplicator::BaseReplicator(config &args)
{
  id = rand()%100000;

  state   = EMBRYO;
  age     = 0;
  gestationTime = 0;

  fitness = 0.0;
  child = NULL;
}

BaseReplicator::~BaseReplicator()
{
  // If I've alloc'd a child who hasn't been born, free it
  config args;
  args.model = modelName;
  if (child and (child->state)==EMBRYO)
    deleteModel(args, child);
}


void BaseReplicator::printDecoder()
{
}

// TODO needs to be more general. Return a string rather than printing to cout?
void BaseReplicator::printData()
{
  for (vector<unsigned short>::iterator it = data.begin() ; it != data.end(); ++it)
    cout << *it << ",";
  cout << endl;
}

void BaseReplicator::print()
{
  cout << "ID: " << id << endl;
  cout << "Decoder:" << endl;
  printDecoder();
  cout << "Data:" << endl;
  printData();
}


// New random data for orphan
void BaseReplicator::newData(config &args)
{
  data.resize(initialDataSize);
  int i;
  for (i=0; i<initialDataSize; i++)
    data[i] = rand()%alphabetSize;
}

// To override
void BaseReplicator::newDecoder(config &args)
{
}

// New (orphan) entity
void BaseReplicator::newEntity(config &args)
{
  newData(args);
  newDecoder(args);
}


// To override. Called at the start of each decoding cycle.
void BaseReplicator::initializeDecoding(config &args)
{
}


// Copy data from the parent to the child at the start of reproduction.
// TODO: re-implement. Not sure this is the best scheme for adding noise
vector<unsigned short> BaseReplicator::copyData(config &args)
{
  vector<unsigned short> dataCopy (data);

  // TODO: I changed the probablities
  // Insert with probability 0.5
  if (rand()%20==0 and data.size() < maxDataSize)
    dataCopy.insert(dataCopy.begin()+rand()%(dataCopy.size()), rand()%alphabetSize);
  // Delete with probability 0.5
  if (rand()%20==0 and data.size() > minDataSize)
    dataCopy.erase(dataCopy.begin()+rand()%(dataCopy.size()));
  // Change with probability 0.5
  if (rand()%20==0)
    dataCopy.at(rand()%(dataCopy.size())) = rand()%alphabetSize;

  return dataCopy;
}


// To override. Iteratively called during the decode cycle.
void BaseReplicator::decode(config &args)
{
  // The default behaviour is that the data and the `phenotype' are the same thing
  copy(child->data.begin(), child->data.end(), back_inserter(child->bodySpecification));
  state = REPRODUCED;
}


// Iteratively called by the controller. What we do here depends on our state, which is mostly controlled by this base class.
// Roughly: 1) Create child and copy data to it. 2) The subclass decodes the child's data and interprets the result as a decoder and a body specification. It gives both to the child. 3) Give child same fitness level as us and both go to state start.
void BaseReplicator::update(config &args)
{
  age++;
  gestationTime++;

  switch(state)
  {
    case START:

      // Create new child
      gestationTime = 0;
      child = newModel(args);

      // Copy data to child
      child->data = copyData(args);

      state = DECODING;
      initializeDecoding(args);
      break;

    case DECODING:

      // Subclass implements this. When it's finished decoding, we expect it to
      // change state to REPRODUCED, and to have given the child a decoding function
      // and body specification
      decode(args);

      break;

    case REPRODUCED:
      
      child->fitness = fitness;
      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle
      
      break;

    default: break;
  }

}
