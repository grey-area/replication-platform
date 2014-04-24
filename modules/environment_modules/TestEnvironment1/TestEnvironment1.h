#include "BaseEnvironment.h"

class TestEnvironment1 : public BaseEnvironment
{
 public:

  void interpretBody(config&,int,int,int);
  void updateFitnesses(config&);

  TestEnvironment1(config&);
  ~TestEnvironment1();

 private:

};
