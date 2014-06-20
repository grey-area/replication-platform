using namespace std;
#include <iostream>
#include "OrgGA.h"

/*

  OrgGA development mechanism.
  Created on 28/4/2014 by andrew

 */

// Print the details of the particular developmentProcess being used by this organism.
void OrgGA::printDevelopmentProcess(ofstream &stream)
{
}

// Create a new developmentProcess for an orphan organism.
void OrgGA::initializeOrphanDevelopmentProcess(config &args)
{
}

// Called at the start of each reproduction cycle. Do any per-cycle
// initialization.
void OrgGA::initializeDevelopmentProcess(config &args)
{
}

// Called iteratively. When development is complete, set the developmentProcess and the
// body specification of the next generation and set state to REPRODUCED.
void OrgGA::updateDevelopment(config &args)
{
  child->somaSpecification = child->copyGenome(args);

  state = REPRODUCED;
}

// Initialize any mechanism-specific variables. Set variables from
// command line/config file.
OrgGA::OrgGA(config &args, globalVars &global) : BaseOrganism(args, global)
{
  // Setting variables from command line/config file
  //if ( args.organismArgs.count("example") )
  //  example = args.organismArgs["example"];
}

OrgGA::~OrgGA()
{
}
