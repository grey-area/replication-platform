using namespace std;
#include <iostream>

#include "EnvRepeatedCycle.h"


// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future, and maybe evaluate it now
void EnvRepeatedCycle::interpretBody(config &args, int x, int y, int t)
{
  BaseDevMechanism *rep = (*grid)[x][y];
  rep->fitness = 0.0;
  rep->score   = 0.0;

  int cycle = 1000000;
  if(args.envArgs.count("switching-rate"))
    cycle = atoi(args.envArgs["switching-rate"].c_str());

  // Changed so that '3' is never rewarded, as '33' is used as a punctation mark in the DevL1 development mechanism
  int toReward = ((t/cycle)%5)+4;
  //int toReward = 3;
  //int toReward = 20;

  // Reward for symbols
  int count = 0;
  for (vector <unsigned short>::iterator it = rep->bodySpecification.begin(); it!=rep->bodySpecification.end() and count<150; ++it,++count)
  {
    if (*it == toReward)
    {
      rep->fitness += 1.0;
      rep->score   += 1.0;
    }  
  }
  functionEvaluations += 1;

  // Reward for long gestation (to a point)
  /*if (rep->gestationTime > 10)
    rep->fitness += 10;
  else
    rep->fitness += rep->gestationTime;

  // Reward for short genome
  rep->fitness -= rep->data.size() / 2;*/
}


// Maybe evaluate the fitnesses of every entity
void EnvRepeatedCycle::updateFitnesses(config &args)
{

/*for (vector<vector<BaseDevMechanism*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseDevMechanism*>::iterator j = i->begin(); j != i->end();++j)
    {
      // Need to dereference j twice
    }
    }*/

}


EnvRepeatedCycle::EnvRepeatedCycle(config &args) : BaseEnvironment(args)
{
}

EnvRepeatedCycle::~EnvRepeatedCycle()
{
}
