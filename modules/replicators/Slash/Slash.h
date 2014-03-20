#include "BaseReplicator.h"
#include "SlashA.hpp"

class Slash : public BaseReplicator
{
public:
  Slash(config &args);
  ~Slash();
};
