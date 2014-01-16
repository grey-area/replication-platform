using namespace std;
#include <iostream>
#include "Replicator1.h"

Replicator1::Replicator1()
{
}

Replicator1::~Replicator1()
{
}

void Replicator1::update()
{
  child = new Replicator1;
}
