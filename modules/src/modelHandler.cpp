using namespace std;
#include <iostream>
#include "modelHandler.h"

// Replicators
#include "Replicator1.h"
#include "Grammar1.h"

// Environments
#include "TestEnvironment1.h"

// TODO replace this setup with constuctor templates/factory constructors?
// TODO this and the Makefile ought to be created by a thing that refers to a database of models and environments
// That thing should also be able to create new template models/environments and put the files in place, with boilerplate code and instructions in comments

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

BaseEnvironment* newEnvironment(string environment, int debug)
{
  BaseEnvironment *e;

  if (environment=="TestEnvironment1")
    e = new TestEnvironment1(debug);
  else
    e = new BaseEnvironment(debug);

  return e;
}

void deleteEnvironment(string environment, BaseEnvironment* environmentObject, int debug)
{
  if (environment=="TestEnvironment1")
    delete ((TestEnvironment1*)environmentObject);
  else
    delete environmentObject;
}
