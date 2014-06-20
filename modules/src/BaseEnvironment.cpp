using namespace std;

#include "BaseEnvironment.h"

BaseEnvironment::BaseEnvironment(config &args)
{
  functionEvaluations = 0;
}

BaseEnvironment::~BaseEnvironment()
{
}


// Given the coordinates of a new entity, create whatever we need to in order to evaluate its metabolism immediately and/or in the future, and maybe evaluate it now
void BaseEnvironment::interpretBody(config &args, globalVars &global, int x, int y)
{
  global.grids[global.gridIndex][x][y]->metabolism = 0.0;
  functionEvaluations += 1;
}

// Maybe evaluate the metabolism of each entity
void BaseEnvironment::updateMetabolisms(config &args, globalVars &global)
{
/*for (vector<vector<BaseOrganism*> >::iterator i = grid->begin(); i != grid->end();++i) 
  {
    for (vector<BaseOrganism*>::iterator j = i->begin(); j != i->end();++j)
    {
      // Need to dereference j twice
    }
    }*/

}


