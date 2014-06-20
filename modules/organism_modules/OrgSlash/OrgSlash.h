#include "BaseOrganism.h"
#include "OrgSlashA.hpp"

class OrgSlash : public BaseOrganism
{
public:

  // these should probably be static, but setting them might be awkward
  int alphabet;
  OrgSlashA::InstructionSet *iset_ptr;
  unsigned int genomeTapeLength;
  unsigned int labelTapeLength;
  unsigned int maxSecondsRuntime;

  OrgSlashA::MemCore *memcore_ptr;
  OrgSlashA::ByteCode bc;
  vector <double> inputBuffer;
  vector <double> outputBuffer;

  void printDevelopmentProcess();
  void initializeOrphanDevelopmentProcess(config&);
  void initializeDevelopmentProcess(config&);
  void updateDevelopment(config&);

  OrgSlash(config &args);
  ~OrgSlash();
};
