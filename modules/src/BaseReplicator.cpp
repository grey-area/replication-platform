using namespace std;

#include <iostream>
#include "BaseReplicator.h"
#include "modelHandler.h"

std::string BaseReplicator::modelName;

BaseReplicator::BaseReplicator()
{
  state = EMBRYO;

  age     = 0;

  data = NULL;

  decodedData = NULL;
  bodySpecification = NULL;
  fitness = 0.0;

  child = NULL;
}

BaseReplicator::~BaseReplicator()
{
  if (data)
    delete [] data;

  if (decodedData)
    delete [] decodedData;

  if (bodySpecification)
    delete [] bodySpecification;

  // If I've alloc'd a child who hasn't been born, free it
  if (child and (child->state)==EMBRYO)
  {
    deleteModel(modelName, child);
  }
}

void BaseReplicator::initializeDecoding()
{
}

unsigned char *BaseReplicator::copyData()
{
  unsigned char *newData;
  return NULL;
}

void BaseReplicator::decode()
{
  state = REPRODUCED;
}

void BaseReplicator::update()
{

  switch(state)
  {
    case START:

      // If we've just been created, we'll have a body specification.
      // We can safely free it, as the environment will have already interpretted it.
      if (bodySpecification)
	delete [] bodySpecification;
  
      // Create new child
      child = newModel(modelName);
      // Copy data to child
      child->data = copyData();

      state = DECODING;
      initializeDecoding();
      break;

    case DECODING:

      decode();
      // Subclass implements this. When it's finished decoding, we expect it to
      // change state to REPRODUCED, and to have given the child a decoding function
      // and body specification. It will have also alloc'd decodedData
      break;

    case REPRODUCED:

      if (decodedData)
	delete [] decodedData;

      child->fitness = fitness;

      child->state   = START; // No longer an embryo!

      state          = START; // Finished reproduction cycle
      break;

    default: break;
  }

}
