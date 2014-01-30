#include "BaseReplicator.h"
#include <vector>

#ifndef BASEENVIRONMENT_H
#define BASEENVIRONMENT_H

class BaseEnvironment
{

public:

  std::vector <std::vector <BaseReplicator*> > *grid;

  virtual void interpretBody(int,int,int,int);
  virtual void updateFitnesses(int);

  BaseEnvironment(int);
  ~BaseEnvironment();
};

#endif /* BASEENVIRONMENT_H */
