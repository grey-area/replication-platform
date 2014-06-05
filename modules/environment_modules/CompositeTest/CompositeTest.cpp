using namespace std;
#include <iostream>

#include "CompositeTest.h"

// Given the coordinates of a new entity, create whatever we need to in order to evaluate its fitness immediately and/or in the future
void CompositeTest::interpretBody(config &args, globalVars &global, int x, int y, int t)
{
  functionEvaluations += 1;

  BaseDevMechanism *rep = global.grids[global.gridIndex][x][y];
  rep->fitness = 0.0;

  int cycle = 10000000;
  if(args.envArgs.count("switching-rate"))
    cycle = atoi(args.envArgs["switching-rate"].c_str());

  int toReward = ((t/cycle)%10) + 20;

  int type = 0;
  int digits = 1;

  if(args.envArgs.count("type"))
    type = atoi(args.envArgs["type"].c_str());

  int maxDigit = 5;
  switch (type)
  {
    case 1:
      digits = maxDigit; break;
    case 2:
      if (t<100000)
	digits = 1;
      else
	digits = maxDigit;
      break;
    case 3:
      digits = 1 + t/20000;
      if (digits > maxDigit)
	digits = maxDigit;
      break;
    case 4:
      digits = 1 + t/40000;
      if (digits > maxDigit)
	digits = maxDigit;
      break;
  }

  int count = 0;
  int failed = 0;
  float dFitness = 0.0;
  for (vector <unsigned short>::iterator it = rep->bodySpecification.begin(); it!=rep->bodySpecification.end() and count<30; ++it,++count)
  {
    if (count%digits==0)
    {
      rep->fitness += dFitness;
      dFitness = 0.0;
      failed = 0;
    }

    if (*it == toReward and not failed)
      dFitness += 1.0;
    else
    {
      rep->fitness -= dFitness;
      dFitness = 0.0;
      failed = 1;
    }
  }
}

void CompositeTest::updateFitnesses(config &args, globalVars &global)
{
}

CompositeTest::CompositeTest(config &args) : BaseEnvironment(args)
{
}

CompositeTest::~CompositeTest()
{
}
