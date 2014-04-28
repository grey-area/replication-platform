#include "BaseDevMechanism.h"
#include "DevSlashA.hpp"

class DevSlash : public BaseDevMechanism
{
public:

  // these should probably be static, but setting them might be awkward
  int alphabet;
  DevSlashA::InstructionSet *iset_ptr;
  unsigned int dataTapeLength;
  unsigned int labelTapeLength;
  unsigned int maxSecondsRuntime;

  DevSlashA::MemCore *memcore_ptr;
  DevSlashA::ByteCode bc;
  vector <double> inputBuffer;
  vector <double> outputBuffer;

  void printDecoder();
  void initializeOrphanDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

  DevSlash(config &args);
  ~DevSlash();
};
