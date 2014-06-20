using namespace std;
#include <iostream>
#include "OrgSlash.h"

/*
  This was an attempt to use the OrgSlash/A language as the development mechanism. Didn't really work.
  Pick up again at some point?

 */

void OrgSlash::printDevelopmentProcess()
{
}

// Create a new developmentProcess, for an orphan cell
void OrgSlash::initializeOrphanDevelopmentProcess(config &args)
{
  //cout << "1" << endl;
  // make a random bytecode vector
  bc.clear();
  //cout << "2" << endl;
  for(int i=0; i<initialGenomeSize*10; ++i)
    bc.push_back(rand()%alphabet);
  //cout << "3" << endl;
}

// Done at the start of each reproduction cycle
void OrgSlash::initializeDevelopmentProcess(config &args)
{
  //cout << "4" << endl;
  OrgSlash *c = ((OrgSlash*)child);

  //cout << "5" << endl;
  // Copy child's genome to child's inputBuffer
  copy(c->genome.begin(), c->genome.end(), back_inserter(c->inputBuffer));

  //cout << "child's input buffer:" << endl;
  //for(vector<double>::iterator it=c->inputBuffer.begin(); it!=c->inputBuffer.end(); ++it)
  //  cout << *it << ";";
  //cout << endl;

  //cout << "6" << endl;
  if (memcore_ptr)
  {
    free (memcore_ptr);
    memcore_ptr = NULL;
  }
  memcore_ptr = new OrgSlashA::MemCore(genomeTapeLength, labelTapeLength, c->inputBuffer, c->outputBuffer);
  //cout << "7" << endl;
}

// Apply the developmentProcess (not iteratively, in this case)
void OrgSlash::updateDevelopment(config &args)
{
  //cout << "8" << endl;
  OrgSlash *c = ((OrgSlash*)child);

  //cout << "9" << endl;
  //for(vector<OrgSlashA::ByteCode_Type>::iterator it=bc.begin(); it!=bc.end(); ++it)
  //  cout << *it << ";";
  //cout << endl;

  try {
    if (bc.size() > 0)
      bool failed = OrgSlashA::runByteCode(*iset_ptr, *memcore_ptr, bc, args.seed, maxSecondsRuntime, -1);
    else
      c->initializeOrphanDevelopmentProcess(args);
  }
  catch(...)
  {
    c->initializeOrphanDevelopmentProcess(args);
  }

  //cout << "10" << endl;
  c->bc.clear();
  //cout << "11" << endl;
  c->somaSpecification.clear();

  //cout << "11" << endl;
  bool body = false;
  // Now the child's output buffer (of doubles) holds the output. Split into two vectors; a vector of ints for the child's bytecode and a vector of unsigned chars for the child's somaSpecification
  for(vector<double>::iterator it = c->outputBuffer.begin(); it != c->outputBuffer.end(); ++it)
  {
    //cout << "12" << endl;
    if (not body)
    {
      unsigned int instruction = ((unsigned int)(*it))%alphabet; // TODO bad casting
      if (instruction==0)
	body=true;
      else
	c->bc.push_back(instruction);
      //cout << "13" << endl;
    }
    else
    {
      //cout << "14" << endl;
      c->somaSpecification.push_back( ((unsigned short)(*it))%256 );
      //cout << "15" << endl;
    }
  }

  /*cout << endl;
  cout << "I am " << id << ", with child " << c->id << endl;
  cout << "   bytecode: " << endl;
  for (vector<OrgSlashA::ByteCode_Type>::iterator it=bc.begin(); it!=bc.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   genome: " << endl;
  for (vector<unsigned short>::iterator it=genome.begin(); it!=genome.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's genome: " << endl;
  for (vector<unsigned short>::iterator it=c->genome.begin(); it!=c->genome.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's input buffer: " << endl;
  for (vector<double>::iterator it=c->inputBuffer.begin(); it!=c->inputBuffer.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's output buffer: " << endl;
  for (vector<double>::iterator it=c->outputBuffer.begin(); it!=c->outputBuffer.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's bytecode: " << endl;
  for (vector<OrgSlashA::ByteCode_Type>::iterator it=c->bc.begin(); it!=c->bc.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's body spec: " << endl;
  for (vector<unsigned short>::iterator it=c->somaSpecification.begin(); it!=c->somaSpecification.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << endl;
  */

  // TODO handle the case where execution fails

  state = REPRODUCED;
}

OrgSlash::OrgSlash(config &args) : BaseOrganism(args)
{
  alphabet = 29;
  genomeTapeLength = maxGenomeSize * 20;
  labelTapeLength = maxGenomeSize;
  maxSecondsRuntime = 1;

  //OrgSlashA::InstructionSet iset(alphabet);
  if (iset_ptr)
  {
    free (iset_ptr);
    iset_ptr = NULL;
  }
  iset_ptr = new OrgSlashA::InstructionSet(alphabet);
  alphabet += 28;
  iset_ptr->insert_DIS_full();
  //cout << iset_ptr->size() << endl;
  //iset_ptr = &iset;
}

OrgSlash::~OrgSlash()
{

  if (iset_ptr)
  {
    free (iset_ptr);
    iset_ptr = NULL;
  }

  if (memcore_ptr)
  {
    free (memcore_ptr);
    memcore_ptr = NULL;
  }
}
