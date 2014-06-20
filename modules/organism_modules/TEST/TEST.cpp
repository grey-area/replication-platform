using namespace std;
#include <iostream>
#include "TEST.h"

/*

  TEST development mechanism.
  Created on 16/6/2014 by andrew

 */

// Print the details of the particular developmentProcess being used by this organism.
void TEST::printDevelopmentProcess(ofstream &stream)
{
}

// Create a new developmentProcess for an orphan organism.
void TEST::initializeOrphanDevelopmentProcess(config &args)
{
}

// Called at the start of each reproduction cycle. Do any per-cycle
// initialization.
void TEST::initializeDevelopmentProcess(config &args)
{
}

// Called iteratively. When development is complete, set the developmentProcess and the
// body specification of the next generation and set state to REPRODUCED.
void TEST::updateDevelopment(config &args)
{

  //state = REPRODUCED;
}

// Initialize any mechanism-specific variables. Set variables from
// command line/config file.
TEST::TEST(config &args) : BaseOrganism(args)
{
  // Setting variables from command line/config file
  //if ( args.organismArgs.count("example") )
  //  example = args.organismArgs["example"];
}

TEST::~TEST()
{
}
