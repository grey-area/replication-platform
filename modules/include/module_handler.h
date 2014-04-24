#include "BaseDevMechanism.h"
#include "BaseEnvironment.h"
#include <string>
#include "config.h"

BaseDevMechanism* newModel(config&);
void deleteModel(config&, BaseDevMechanism*);
BaseEnvironment* newEnvironment(config&);
void deleteEnvironment(config&, BaseEnvironment*);
