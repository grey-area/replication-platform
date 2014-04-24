#include "BaseReplicator.h"
#include <map>

class Atomic : public BaseReplicator
{
public:

  Atomic(config &args);
  ~Atomic();

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
  void newDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

};
