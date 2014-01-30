#include "BaseReplicator.h"
#include "BaseEnvironment.h"
#include <string>

BaseReplicator* newModel(string, int);
void deleteModel(string, BaseReplicator*, int);
BaseEnvironment* newEnvironment(string, int);
void deleteEnvironment(string, BaseEnvironment*, int);
