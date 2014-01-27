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


BaseReplicator::BaseReplicator()
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
  {
    deleteModel(modelName, child);
  }
}

void BaseReplicator::printData(vector<unsigned short> d)
{
  for (vector<unsigned short>::iterator it = d.begin() ; it != d.end(); ++it)
    cout << ' ' << *it;
  cout << '\n';
}

void BaseReplicator::newData()
{
  data.resize(initialDataSize);
  int i;
  for (i=0; i<initialDataSize; i++)
    data[i] = rand()%alphabetSize;
}

// To override
void BaseReplicator::newDecoder()
{
}

void BaseReplicator::newEntity()
{
  newData();
  newDecoder();
}

// To override
void BaseReplicator::initializeDecoding(int debug)
{
}

// TODO: re-implement
vector<unsigned short> BaseReplicator::copyData()
{
  vector<unsigned short> dataCopy (data);
  //dataCopy.insert(dataCopy.end(), data.begin(), data.end());

  // TODO: I changed the probablities to 0.2

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

// To override
void BaseReplicator::decode(int debug)
{
  state = REPRODUCED;
}

void BaseReplicator::update(int debug)
{
  age++;
  gestationTime++;

  //cout << "\tState: " << state << endl;;
  switch(state)
  {
    case START:

      // Create new child
      gestationTime = 0;
      child = newModel(modelName);
      //cout << "\t\tCreated child " << child->id << endl;
      // Copy data to child
      child->data = copyData();

      state = DECODING;
      initializeDecoding(debug);
      break;

    case DECODING:

      decode(debug);
      // Subclass implements this. When it's finished decoding, we expect it to
      // change state to REPRODUCED, and to have given the child a decoding function
      // and body specification
      break;

    case REPRODUCED:

      child->fitness = fitness;
      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle
      break;

    default: break;
  }

}
