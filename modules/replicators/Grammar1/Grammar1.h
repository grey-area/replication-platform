#include "BaseReplicator.h"
#include <list>

class Grammar1 : public BaseReplicator
{
public:

  int ruleAlphabet;
  vector<vector<unsigned short> > rules;
  list<unsigned short> decodedData;
  list<unsigned short>::iterator prevIt;

  Grammar1();
  ~Grammar1();
  
  void newDecoder();
  void initializeDecoding(int);
  void decode(int);
};
