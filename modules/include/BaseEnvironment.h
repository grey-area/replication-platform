#include "BaseReplicator.h"
#include "config.h"
#include <vector>

#ifndef BASEENVIRONMENT_H
#define BASEENVIRONMENT_H

class BaseEnvironment
{

public:

  std::vector <std::vector <BaseReplicator*> > *grid;

  int functionEvaluations;

  virtual void interpretBody(config&,int,int,int);
  virtual void updateFitnesses(config&);

  BaseEnvironment(config&);
  ~BaseEnvironment();
};

#endif /* BASEENVIRONMENT_H */
