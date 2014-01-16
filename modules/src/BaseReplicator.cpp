using namespace std;
#include <iostream>
#include "BaseReplicator.h"

BaseReplicator::BaseReplicator()
{
  fitness = 1;
  age     = 0;
  bodySpecification = NULL;
  child = NULL;
}

BaseReplicator::~BaseReplicator()
{
}

void BaseReplicator::update()
{
}

void BaseReplicator::baseUpdate()
{
  child = NULL;
  age++;
}
