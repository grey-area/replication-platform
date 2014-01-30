#include "BaseReplicator.h"
#include <list>

class Grammar1 : public BaseReplicator
{
 public:

  Grammar1(int);
  ~Grammar1();

 private:

  int ruleAlphabet;
  vector <vector<unsigned short> > rules;
  list<unsigned short> decodedData;
  list<unsigned short>::iterator prevIt;
  bool prevChanged;
  void interpret(Grammar1*, int);

  void newDecoder(int);
  void initializeDecoding(int);
  void decode(int);

};
