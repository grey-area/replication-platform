using namespace std;
#include "TestEnvironment1.h"

#include <iostream>

// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future, and maybe evaluate it now
void TestEnvironment1::interpretBody(int x, int y, int t, int debug)
{
  BaseReplicator *rep = (*grid)[x][y];
  rep->fitness = 0.0;

  int cycle = 2000;
  int toReward = (t%cycle>(cycle/2) ? 20 : 21);

  int count = 0;
  for (vector <unsigned short>::iterator it = rep->bodySpecification.begin(); it!=rep->bodySpecification.end() and count<30; ++it,++count)
  {
    if (*it == toReward)
      rep->fitness += 1.0;
  }
}


// Maybe evaluate the fitnesses of every entity
void TestEnvironment1::updateFitnesses(int debug)
{

/*for (vector<vector<BaseReplicator*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseReplicator*>::iterator j = i->begin(); j != i->end();++j)
    {
      // Need to dereference j twice
    }
    }*/

}


TestEnvironment1::TestEnvironment1(int debug) : BaseEnvironment(debug)
{
}

TestEnvironment1::~TestEnvironment1()
{
}
