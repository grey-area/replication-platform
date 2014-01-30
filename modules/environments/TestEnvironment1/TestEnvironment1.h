#include "BaseEnvironment.h"

class TestEnvironment1 : public BaseEnvironment
{
 public:

  void interpretBody(int,int,int,int);
  void updateFitnesses(int);

  TestEnvironment1(int);
  ~TestEnvironment1();

 private:

};
