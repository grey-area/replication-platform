#include "BaseEnvironment.h"

class EnvRepeatedCycle : public BaseEnvironment
{
 public:

  void interpretBody(config&,int,int);
  void updateFitnesses(config&);

  EnvRepeatedCycle(config&);
  ~EnvRepeatedCycle();

 private:

};
