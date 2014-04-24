#include "BaseDevMechanism.h"
#include <list>

class DevGrammar1 : public BaseDevMechanism
{
 public:

  void printDecoder(ofstream &stream);

  DevGrammar1(config&);
  ~DevGrammar1();

 private:

  int ruleAlphabet;
  vector <vector<unsigned short> > rules;
  list<unsigned short> decodedData;
  list<unsigned short>::iterator prevIt;
  bool prevChanged;
  void interpret(config&, DevGrammar1*);

  void newDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

};
