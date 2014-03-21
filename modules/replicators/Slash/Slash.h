#include "BaseReplicator.h"
#include "SlashA.hpp"

class Slash : public BaseReplicator
{
public:

  // these should probably be static, but setting them might be awkward
  int alphabet;
  SlashA::InstructionSet *iset_ptr;
  unsigned int dataTapeLength;
  unsigned int labelTapeLength;
  unsigned int maxSecondsRuntime;

  SlashA::MemCore *memcore_ptr;
  SlashA::ByteCode bc;
  vector <double> inputBuffer;
  vector <double> outputBuffer;

  void printDecoder();
  void newDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

  Slash(config &args);
  ~Slash();
};
