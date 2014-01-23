using namespace std;
#include <iostream>
#include "modelHandler.h"
#include "Replicator1.h"

BaseReplicator* newModel(string model)
{
  BaseReplicator *m;

  if (model=="Replicator1")
    m = new Replicator1;
  else                         
    m = new BaseReplicator;

  return m;
}

void deleteModel(string model, BaseReplicator* modelObject)
{
  if (model=="Replicator1")
    delete ((Replicator1*)modelObject);
  else
    delete modelObject;
}
