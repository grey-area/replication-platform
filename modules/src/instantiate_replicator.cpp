using namespace std;
#include "instantiate_replicator.h"
#include "Replicator1.h"
#include "Replicator2.h"

BaseReplicator* instantiate_replicator(string replicator)
{
  BaseReplicator *m;

  if (replicator=="Replicator1")      m = new Replicator1;
  else if (replicator=="Replicator2") m = new Replicator2;
  else                                m = new BaseReplicator;

  return m;
}
