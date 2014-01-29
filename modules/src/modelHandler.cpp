using namespace std;
#include <iostream>
#include "modelHandler.h"
#include "Replicator1.h"
#include "Grammar1.h"

// TODO replace this setup with constuctor templates/factory constructors?

BaseReplicator* newModel(string model, int debug)
{
  BaseReplicator *m;

  if (model=="Replicator1")
    m = new Replicator1(debug);
  else if (model=="Grammar1")
    m = new Grammar1(debug);
  else                         
    m = new BaseReplicator(debug);

  return m;
}

void deleteModel(string model, BaseReplicator* modelObject, int debug)
{
  if (model=="Replicator1")
    delete ((Replicator1*)modelObject);
  else if (model=="Grammar1")
    delete ((Grammar1*)modelObject);
  else
    delete modelObject;
}
