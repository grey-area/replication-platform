using namespace std;
#include <iostream>

#include "EnvRepeatedCycle.h"


// Given the coordinates of a new entity, create whatever we need to in order to evaluate its metabolism immediately and/or in the future, and maybe evaluate it now
void EnvRepeatedCycle::interpretBody(config &args, globalVars &global, int x, int y)
{
  // TODO should probably pass the grid in. When interpret?
  BaseOrganism *rep = global.grids[1-global.gridIndex][x][y];
  rep->metabolism = 0.0;

  int cycle = 1000000;
  if(args.environmentArgs.count("switching-rate"))
    cycle = atoi(args.environmentArgs["switching-rate"].c_str());

  // Changed so that '3' is never rewarded, as '33' is used as a punctation mark in the OrgL1 development mechanism
  int toReward = ((functionEvaluations/cycle)%5)+4;
  //int toReward = 3;
  //int toReward = 20;

  // Reward for symbols
  int count = 0;
  for (vector <unsigned short>::iterator it = rep->somaSpecification.begin(); it!=rep->somaSpecification.end() and count<150; ++it,++count)
  {
    if (*it == toReward)
    {
      rep->metabolism += 1.0;
    }  
  }
  functionEvaluations += 1;

  // Reward for long gestation (to a point)
  /*if (rep->gestationTime > 10)
    rep->metabolism += 10;
  else
    rep->metabolism += rep->gestationTime;

  // Reward for short genome
  rep->metabolism -= rep->genome.size() / 2;*/
}


// Maybe evaluate the metabolisms of every entity
void EnvRepeatedCycle::updateMetabolisms(config &args, globalVars &global)
{

/*for (vector<vector<BaseOrganism*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseOrganism*>::iterator j = i->begin(); j != i->end();++j)
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
