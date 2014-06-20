using namespace std;
#include <iostream>
#include <algorithm>
#include "OrgAtomic.h"

/*
  Implemented a language based on OrgSlash/A (based on Avida) to use as a development mechanism.
  Didn't really work. Pick up again at some point?
 */

void OrgAtomic::printDevelopmentProcess()
{
}

// Create a new developmentProcess, for an `orphan' cell
void OrgAtomic::initializeOrphanDevelopmentProcess(config &args)
{
  code.clear();
  for(int i=0; i<initialGenomeSize*5; ++i)
    code.push_back(rand()%isetSize);
}

// Done at the start of each reproduction cycle
void OrgAtomic::initializeDevelopmentProcess(config &args)
{
  OrgAtomic *c = ((OrgAtomic*)child);

  // Copy child's genome to my tape
  tape.clear();
  copy(child->genome.begin(), child->genome.end(), back_inserter(tape));
  output.clear();
  A = 0;
  I = 0;
  ip = code.begin();
  labels.clear();

  loop_ptrs.clear();
  loopTimes.clear();
}

void OrgAtomic::execute(unsigned int ins)
{
  ip++;

  if (ins < 21)
    I = ins;
  else
  {
    switch(ins)
    {
    case 21: // AtoI
      I = A; break;
    case 22: // ItoA
      A = I; break;
    case 23: // inc
      A += 1; break;
    case 24: // dec
      A -= 1; break;
    case 25: // load
      if (I < tape.size())
	A = tape[I];
      break;
    case 26: // save
      if (I >= tape.size())
	tape.resize(min(I+1, maxTapeSize));
      if (I < tape.size())
	tape[I] = A;
      break;
    case 27: // swap
      {
      unsigned int temp = A;
      A = I;
      I = temp; break;}
    case 28: // cmp
      if (I < tape.size())
	A = (A==tape[I] ? 1 : 0);
      else
	A = 0;
      break; 
    case 29: // label
      if (I < maxTapeSize)
	labels[I] = ip;
      break;
    case 30: // gotoif
      if (A > 0 and labels.count(I) > 0)
	ip = labels[I];
      break;
    case 31: // jumpif
      if (A > 0)
      {
	vector<unsigned int>::iterator jump = find(ip, code.end(), 32);
	if (jump != code.end())
	  ip = jump;
      }
      break;
    // 32 is a `jumphere' and does nothing on its own

    case 33: // loop 
      if (loop_ptrs.size() < 50)
      {
	loop_ptrs.push_back(ip);
	loopTimes.push_back(I);
      }
      break;
    case 34: // endloop
      if (loop_ptrs.size()>0)
      {
	if (loopTimes.back()>0)
	{
	  loopTimes.back() -= 1;
	  ip = loop_ptrs.back();
	}
	else
	{
	  loop_ptrs.pop_back();
	  loopTimes.pop_back();
	}
      }

      break;
    case 35: // add
      if (I < tape.size())
	A += tape[I];
      break;
    case 36: // sub
      if (I < tape.size())
	A -= tape[I]; 
      break; 
    case 37: // mul
      if (I < tape.size())	
	A *= tape[I];
      break;
    case 38: // div
      if (I < tape.size() and tape[I] != 0)
	A /= tape[I];
      break; 
    case 39: // rand
      A = rand(); break;
    // 40 in NOP
    case 41: // output
      output.push_back(A); break;
    default: break;
    }
  }
}

// Iteratively run the thing
void OrgAtomic::updateDevelopment(config &args)
{
  OrgAtomic *c = (OrgAtomic*)child;

  // Execute the instruction at the instruction pointer, increment the instruction pointer
  execute(*ip);

  // If we've finished executing
  if (ip == code.end())
  {
    state=REPRODUCED;

    int separator[] = {40,40,40};    
    vector<unsigned int>::iterator separatorLocation = search(output.begin(), output.end(), separator, separator+3);

    int afterSeparator = 0;
    for(vector<unsigned int>::iterator it=output.begin(); it!=output.end(); ++it)
    {
      if (afterSeparator > 0 and afterSeparator < 3)
	afterSeparator += 1;
      if (it == separatorLocation)
	afterSeparator += 1;
      if (afterSeparator == 0)
	c->code.push_back((*it)%isetSize);
      else if (afterSeparator == 3)
	c->somaSpecification.push_back((*it)%256);
    }
  }
}

OrgAtomic::OrgAtomic(config &args, globalVars &global) : BaseOrganism(args, global)
{
  isetSize = 42;
  ip = code.begin();
  A = 0;
  I = 0;
 
  maxTapeSize = maxGenomeSize * 10;
}

OrgAtomic::~OrgAtomic()
{
}
