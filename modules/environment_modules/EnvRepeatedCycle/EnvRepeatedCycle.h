#include "BaseEnvironment.h"

class EnvRepeatedCycle : public BaseEnvironment
{
 public:

  void interpretBody(config&,globalVars&,int,int);
  void updateMetabolisms(config&,globalVars&);

  EnvRepeatedCycle(config&);
  ~EnvRepeatedCycle();

 private:

};
