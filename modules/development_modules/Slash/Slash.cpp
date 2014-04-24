using namespace std;
#include <iostream>
#include "Slash.h"

/*
  This was an attempt to use the Slash/A language as the development mechanism. Didn't really work.
  Pick up again at some point?

 */

void Slash::printDecoder()
{
}

// Create a new decoder, for an orphan cell
void Slash::newDecoder(config &args)
{
  //cout << "1" << endl;
  // make a random bytecode vector
  bc.clear();
  //cout << "2" << endl;
  for(int i=0; i<initialDataSize*10; ++i)
    bc.push_back(rand()%alphabet);
  //cout << "3" << endl;
}

// Done at the start of each reproduction cycle
void Slash::initializeDecoding(config &args)
{
  //cout << "4" << endl;
  Slash *c = ((Slash*)child);

  //cout << "5" << endl;
  // Copy child's data to child's inputBuffer
  copy(c->data.begin(), c->data.end(), back_inserter(c->inputBuffer));

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
  memcore_ptr = new SlashA::MemCore(dataTapeLength, labelTapeLength, c->inputBuffer, c->outputBuffer);
  //cout << "7" << endl;
}

// Apply the decoder (not iteratively, in this case)
void Slash::decode(config &args)
{
  //cout << "8" << endl;
  Slash *c = ((Slash*)child);

  //cout << "9" << endl;
  //for(vector<SlashA::ByteCode_Type>::iterator it=bc.begin(); it!=bc.end(); ++it)
  //  cout << *it << ";";
  //cout << endl;

  try {
    if (bc.size() > 0)
      bool failed = SlashA::runByteCode(*iset_ptr, *memcore_ptr, bc, args.seed, maxSecondsRuntime, -1);
    else
      c->newDecoder(args);
  }
  catch(...)
  {
    c->newDecoder(args);
  }

  //cout << "10" << endl;
  c->bc.clear();
  //cout << "11" << endl;
  c->bodySpecification.clear();

  //cout << "11" << endl;
  bool body = false;
  // Now the child's output buffer (of doubles) holds the output. Split into two vectors; a vector of ints for the child's bytecode and a vector of unsigned chars for the child's bodySpecification
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
      c->bodySpecification.push_back( ((unsigned short)(*it))%256 );
      //cout << "15" << endl;
    }
  }

  /*cout << endl;
  cout << "I am " << id << ", with child " << c->id << endl;
  cout << "   bytecode: " << endl;
  for (vector<SlashA::ByteCode_Type>::iterator it=bc.begin(); it!=bc.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   data: " << endl;
  for (vector<unsigned short>::iterator it=data.begin(); it!=data.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's data: " << endl;
  for (vector<unsigned short>::iterator it=c->data.begin(); it!=c->data.end(); ++it)
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
  for (vector<SlashA::ByteCode_Type>::iterator it=c->bc.begin(); it!=c->bc.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << "   child's body spec: " << endl;
  for (vector<unsigned short>::iterator it=c->bodySpecification.begin(); it!=c->bodySpecification.end(); ++it)
    cout << *it << ";";
  cout << endl;
  cout << endl;
  */

  // TODO handle the case where execution fails

  state = REPRODUCED;
}

Slash::Slash(config &args) : BaseReplicator(args)
{
  alphabet = 29;
  dataTapeLength = maxDataSize * 20;
  labelTapeLength = maxDataSize;
  maxSecondsRuntime = 1;

  //SlashA::InstructionSet iset(alphabet);
  if (iset_ptr)
  {
    free (iset_ptr);
    iset_ptr = NULL;
  }
  iset_ptr = new SlashA::InstructionSet(alphabet);
  alphabet += 28;
  iset_ptr->insert_DIS_full();
  //cout << iset_ptr->size() << endl;
  //iset_ptr = &iset;
}

Slash::~Slash()
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
