#include "BaseReplicator.h"
#include <list>

class Grammar1 : public BaseReplicator
{
public:

  int ruleAlphabet;
  vector<vector<unsigned short> > rules;
  list<unsigned short> decodedData;
  list<unsigned short>::iterator prevIt;
  bool prevChanged;

  Grammar1(int);
  ~Grammar1();
  
  void interpret(Grammar1*, int);
  void newDecoder(int);
  void initializeDecoding(int);
  void decode(int);
};
