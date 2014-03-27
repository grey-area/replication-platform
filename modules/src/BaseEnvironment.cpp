using namespace std;

#include "BaseEnvironment.h"

BaseEnvironment::BaseEnvironment(config &args)
{
  functionEvaluations = 0;
}

BaseEnvironment::~BaseEnvironment()
{
}


// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future, and maybe evaluate it now
void BaseEnvironment::interpretBody(config &args, int x, int y, int t)
{
  (*grid)[x][y]->fitness = 0.0;
  (*grid)[x][y]->score   = 0.0;
  functionEvaluations += 1;
}

// Maybe evaluate the fitnesses of every entity
void BaseEnvironment::updateFitnesses(config &args)
{
/*for (vector<vector<BaseReplicator*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseReplicator*>::iterator j = i->begin(); j != i->end();++j)
    {
      // Need to dereference j twice
    }
    }*/

}


