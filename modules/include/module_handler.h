#include "BaseDevMechanism.h"
#include "BaseEnvironment.h"
#include <string>
#include "config.h"

BaseDevMechanism* newOrganism(config&);
void deleteOrganism(config&, BaseDevMechanism*);
BaseEnvironment* newEnvironment(config&);
void deleteEnvironment(config&, BaseEnvironment*);
