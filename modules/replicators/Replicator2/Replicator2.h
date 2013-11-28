#include "BaseReplicator.h"

class Replicator2 : public BaseReplicator
{
public:
  Replicator2();
  ~Replicator2();
  BaseReplicator* update();
};
