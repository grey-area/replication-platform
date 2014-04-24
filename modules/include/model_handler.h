#include "BaseReplicator.h"
#include "BaseEnvironment.h"
#include <string>
#include "config.h"

BaseReplicator* newModel(config&);
void deleteModel(config&, BaseReplicator*);
BaseEnvironment* newEnvironment(config&);
void deleteEnvironment(config&, BaseEnvironment*);
