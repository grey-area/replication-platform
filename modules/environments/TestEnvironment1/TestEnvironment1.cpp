using namespace std;
#include <iostream>

#include "TestEnvironment1.h"


// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future, and maybe evaluate it now
void TestEnvironment1::interpretBody(config &args, int x, int y, int t)
{
  BaseReplicator *rep = (*grid)[x][y];
  rep->fitness = 0.0;

  int cycle = 1000000;
  if(args.environmentConfig.count("switching-rate"))
    cycle = atoi(args.environmentConfig["switching-rate"].c_str());

  //int toReward = ((t/cycle)%10)+20;
  int toReward = 10;
  //int toReward = 20;

  // Reward for symbols
  int count = 0;
  for (vector <unsigned short>::iterator it = rep->bodySpecification.begin(); it!=rep->bodySpecification.end() and count<300; ++it,++count)
  {
    if (*it == toReward)
      rep->fitness += 1.0;
  }
  functionEvaluations += 1;

  /*// Reward for long gestation (to a point)
  if (rep->gestationTime > 10)
    rep->fitness += 10;
  else
    rep->fitness += rep->gestationTime;

  // Reward for short genome
  rep->fitness -= rep->data.size() / 20;*/
}


// Maybe evaluate the fitnesses of every entity
void TestEnvironment1::updateFitnesses(config &args)
{

/*for (vector<vector<BaseReplicator*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseReplicator*>::iterator j = i->begin(); j != i->end();++j)
    {
      // Need to dereference j twice
    }
    }*/

}


TestEnvironment1::TestEnvironment1(config &args) : BaseEnvironment(args)
{
}

TestEnvironment1::~TestEnvironment1()
{
}
