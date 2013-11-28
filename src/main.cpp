using namespace std;
#include <iostream>
#include "instantiate_replicator.h"

int main()
{
  BaseReplicator *m = instantiate_replicator("Replicator2");
  m->update();
  delete m;

  return 0;
}
