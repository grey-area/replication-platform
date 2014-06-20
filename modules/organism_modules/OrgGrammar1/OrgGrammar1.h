#include "BaseOrganism.h"
#include <list>

class OrgGrammar1 : public BaseOrganism
{
 public:

  void printDevelopmentProcess(ofstream &stream);

  OrgGrammar1(config&);
  ~OrgGrammar1();

 private:

  int ruleAlphabet;
  vector <vector<unsigned short> > rules;
  list<unsigned short> decodedGenome;
  list<unsigned short>::iterator prevIt;
  bool prevChanged;
  void interpret(config&, OrgGrammar1*);

  void initializeOrphanDevelopmentProcess(config&);
  void initializeDevelopmentProcess(config&);
  void updateDevelopment(config&);

};
