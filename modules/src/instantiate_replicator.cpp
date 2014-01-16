using namespace std;
#include "instantiate_replicator.h"
#include "Replicator1.h"

BaseReplicator* instantiate_replicator(string replicator)
{
  BaseReplicator *m;

  if (replicator=="Replicator1")      m = new Replicator1;
  else                                m = new BaseReplicator;

  return m;
}

void delete_replicator(string replicator, BaseReplicator* replicatorObject)
{
  if (replicator=="Replicator1")      delete ((Replicator1*)replicatorObject);
  else                                delete replicatorObject;
}
