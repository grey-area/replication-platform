using namespace std;

#include <iostream>
#include "BaseReplicator.h"
#include "modelHandler.h"
#include <algorithm>
#include <set>

std::string BaseReplicator::modelName;
int BaseReplicator::alphabetSize;
int BaseReplicator::initialDataSize;
int BaseReplicator::minDataSize;
int BaseReplicator::maxDataSize;


BaseReplicator::BaseReplicator(int debug)
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
  if (child and (child->state)==EMBRYO)
    deleteModel(modelName, child, 0);
}


// TODO needs to be more general. Return a string rather than printing to cout?
void BaseReplicator::printData(vector<unsigned short> d)
{
  for (vector<unsigned short>::iterator it = d.begin() ; it != d.end(); ++it)
    cout << ' ' << *it;
  cout << '\n';
}


// New random data for orphan
void BaseReplicator::newData(int debug)
{
  data.resize(initialDataSize);
  int i;
  for (i=0; i<initialDataSize; i++)
    data[i] = rand()%alphabetSize;
}

// To override
void BaseReplicator::newDecoder(int debug)
{
}

// New (orphan) entity
void BaseReplicator::newEntity(int debug)
{
  newData(debug);
  newDecoder(debug);
}


// To override. Called at the start of each decoding cycle.
void BaseReplicator::initializeDecoding(int debug)
{
}


// Copy data from the parent to the child at the start of reproduction.
// TODO: re-implement. Not sure this is the best scheme for adding noise
vector<unsigned short> BaseReplicator::copyData(int debug)
{
  vector<unsigned short> dataCopy (data);
  //dataCopy.insert(dataCopy.end(), data.begin(), data.end());

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
void BaseReplicator::decode(int debug)
{
  state = REPRODUCED;
}


// Iteratively called by the controller. What we do here depends on our state, which is mostly controlled by this base class.
// Roughly: 1) Create child and copy data to it. 2) The subclass decodes the child's data and interprets the result as a decoder and a body specification. It gives both to the child. 3) Give child same fitness level as us and both go to state start.
void BaseReplicator::update(int debug)
{
  age++;
  gestationTime++;

  switch(state)
  {
    case START:

      // Create new child
      gestationTime = 0;
      child = newModel(modelName, debug);

      // Copy data to child
      child->data = copyData(debug);

      state = DECODING;
      initializeDecoding(debug);
      break;

    case DECODING:

      // Subclass implements this. When it's finished decoding, we expect it to
      // change state to REPRODUCED, and to have given the child a decoding function
      // and body specification
      decode(debug);

      break;

    case REPRODUCED:

      child->fitness = fitness;
      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle
      break;

    default: break;
  }

}
