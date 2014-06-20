#include "BaseOrganism.h"
#include <map>

class OrgAtomic : public BaseOrganism
{
public:

  OrgAtomic(config &args, globalVars &global);
  ~OrgAtomic();

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

  void printDevelopmentProcess();
  void initializeOrphanDevelopmentProcess(config&);
  void initializeDevelopmentProcess(config&);
  void updateDevelopment(config&);

};
