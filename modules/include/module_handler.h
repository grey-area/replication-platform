#include "BaseOrganism.h"
#include "BaseEnvironment.h"
#include <string>
#include "config.h"

BaseOrganism* newOrganism(config&, globalVars&);
void deleteOrganism(config&, BaseOrganism*);
BaseEnvironment* newEnvironment(config&);
void deleteEnvironment(config&, BaseEnvironment*);
