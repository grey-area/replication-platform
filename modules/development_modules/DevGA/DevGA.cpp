using namespace std;
#include <iostream>
#include "DevGA.h"

/*

  DevGA development mechanism.
  Created on 28/4/2014 by andrew

 */

// Print the details of the particular decoder being used by this organism.
void DevGA::printDecoder(ofstream &stream)
{
}

// Create a new decoder for an orphan organism.
void DevGA::initializeOrphanDecoder(config &args)
{
}

// Called at the start of each reproduction cycle. Do any per-cycle
// initialization.
void DevGA::initializeDecoding(config &args)
{
}

// Called iteratively. When decoding is complete, set the decoder and the
// body specification of the next generation and set state to REPRODUCED.
void DevGA::decode(config &args)
{
  child->bodySpecification = child->copyData(args);

  state = REPRODUCED;
}

// Initialize any mechanism-specific variables. Set variables from
// command line/config file.
DevGA::DevGA(config &args) : BaseDevMechanism(args)
{
  // Setting variables from command line/config file
  //if ( args.devArgs.count("example") )
  //  example = args.devArgs["example"];
}

DevGA::~DevGA()
{
}
