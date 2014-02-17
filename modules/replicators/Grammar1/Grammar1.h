#include "BaseReplicator.h"
#include <list>

class Grammar1 : public BaseReplicator
{
 public:

  Grammar1(config&);
  ~Grammar1();

 private:

  int ruleAlphabet;
  vector <vector<unsigned short> > rules;
  list<unsigned short> decodedData;
  list<unsigned short>::iterator prevIt;
  bool prevChanged;
  void interpret(config&, Grammar1*);

  void printDecoder();

  void newDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

};
