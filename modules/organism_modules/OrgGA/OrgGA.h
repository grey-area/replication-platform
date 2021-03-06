#include "BaseOrganism.h"

class OrgGA : public BaseOrganism
{
 public:
  OrgGA(config&, globalVars&);
  ~OrgGA();

  // Optional
  //void mutateGenome(config&, vector<unsigned short>&);

 private:
  void printDevelopmentProcess(ofstream&);
  void initializeOrphanDevelopmentProcess(config&);
  void initializeDevelopmentProcess(config&);
  void updateDevelopment(config&);

  /* Mechanism-specific variables. Suggestions:
     - Somewhere to store a working copy of the decoded genome.
     - Variable for initial size of orphan developmentProcesss.
     - Variable for maximum size of decoded genome.
   */

  // Optional
  //void initializeOrphanGenome(config&);
  //void printGenome(ofstream&);
};
