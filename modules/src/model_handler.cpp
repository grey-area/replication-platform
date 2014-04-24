using namespace std;
#include <iostream>
#include "model_handler.h"

// Development modules
#include "Grammar1.h"
#include "Atomic.h"
#include "L1.h"

// Environment modules
#include "TestEnvironment1.h"
#include "CompositeTest.h"

BaseReplicator* newModel(config &args)
{
  BaseReplicator *m;

  if (args.model=="BaseReplicator")
    m = new BaseReplicator(args);
  else if (args.model=="Grammar1")
    m = new Grammar1(args);
  else if (args.model=="Atomic")
    m = new Atomic(args);
  else if (args.model=="L1")
    m = new L1(args);
  else                         
    m = new BaseReplicator(args);

  return m;
}

void deleteModel(config &args, BaseReplicator* modelObject)
{
  if (args.model=="BaseReplicator")
    delete modelObject;
  else if (args.model=="Grammar1")
    delete ((Grammar1*)modelObject);
  else if (args.model=="Atomic")
    delete ((Atomic*)modelObject);
  else if (args.model=="L1")
    delete ((L1*)modelObject);
  else
    delete modelObject;
}

BaseEnvironment* newEnvironment(config &args)
{
  BaseEnvironment *e;

  if (args.environment=="BaseEnvironment")
    e = new BaseEnvironment(args);
  else if (args.environment=="TestEnvironment1")
    e = new TestEnvironment1(args);
  else if (args.environment=="CompositeTest")
    e = new CompositeTest(args);
  else
    e = new BaseEnvironment(args);

  return e;
}

void deleteEnvironment(config &args, BaseEnvironment* environmentObject)
{
  if (args.environment=="BaseEnvironment")
    delete environmentObject;
  else if (args.environment=="TestEnvironment1")
    delete ((TestEnvironment1*)environmentObject);
  else if (args.environment=="CompositeTest")
    delete ((CompositeTest*)environmentObject);
  else
    delete environmentObject;
}
