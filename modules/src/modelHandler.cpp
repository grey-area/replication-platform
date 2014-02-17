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

BaseReplicator* newModel(config &args)
{
  BaseReplicator *m;

  if (args.model=="Replicator1")
    m = new Replicator1(args);
  else if (args.model=="Grammar1")
    m = new Grammar1(args);
  else                         
    m = new BaseReplicator(args);

  return m;
}

void deleteModel(config &args, BaseReplicator* modelObject)
{
  if (args.model=="Replicator1")
    delete ((Replicator1*)modelObject);
  else if (args.model=="Grammar1")
    delete ((Grammar1*)modelObject);
  else
    delete modelObject;
}

BaseEnvironment* newEnvironment(config &args)
{
  BaseEnvironment *e;

  if (args.environment=="TestEnvironment1")
    e = new TestEnvironment1(args);
  else
    e = new BaseEnvironment(args);

  return e;
}

void deleteEnvironment(config &args, BaseEnvironment* environmentObject)
{
  if (args.environment=="TestEnvironment1")
    delete ((TestEnvironment1*)environmentObject);
  else
    delete environmentObject;
}
