#include "BaseDevMechanism.h"
#include <map>

class DevAtomic : public BaseDevMechanism
{
public:

  DevAtomic(config &args, globalVars &global);
  ~DevAtomic();

 private:

  vector<unsigned int> code;
  vector<unsigned int> tape;
  vector<unsigned int> output;
  map<unsigned int, vector<unsigned int>::iterator> labels;
  vector<vector<unsigned int>::iterator> loop_ptrs;
  vector<unsigned int> loopTimes;
  vector<unsigned int>::iterator ip;
  unsigned int A;
  unsigned int I;

  unsigned int isetSize;
  unsigned int maxTapeSize;
  
  void execute(unsigned int);

  void printDecoder();
  void initializeOrphanDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

};
