#include "BaseEnvironment.h"

class CompositeTest : public BaseEnvironment
{
public:

  void interpretBody(config&,int,int,int);
  void updateMetabolisms(config&);


  CompositeTest(config&);
  ~CompositeTest();

private:

};
