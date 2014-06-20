#include "BaseOrganism.h"
#include "config.h"
#include <vector>

#ifndef BASEENVIRONMENT_H
#define BASEENVIRONMENT_H

class BaseEnvironment
{

public:

  int functionEvaluations;

  virtual void interpretBody(config&,globalVars&,int,int);
  virtual void updateMetabolisms(config&,globalVars&);

  BaseEnvironment(config&);
  ~BaseEnvironment();
};

#endif /* BASEENVIRONMENT_H */
