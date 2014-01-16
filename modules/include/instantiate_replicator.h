#include "BaseReplicator.h"
#include <string>

BaseReplicator* instantiate_replicator(string);
void delete_replicator(string, BaseReplicator*);
