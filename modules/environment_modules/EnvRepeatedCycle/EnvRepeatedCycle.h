#include "BaseEnvironment.h"

class EnvRepeatedCycle : public BaseEnvironment
{
 public:

  void interpretBody(config&,globalVars&,int,int);
  void updateFitnesses(config&,globalVars&);

  EnvRepeatedCycle(config&);
  ~EnvRepeatedCycle();

 private:

};
