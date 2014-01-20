#include <string>

#ifndef BASEREPLICATOR_H
#define BASEREPLICATOR_H

#define START       0
#define DECODING    1
#define REPRODUCED  2
#define EMBRYO      9

class BaseReplicator
{

public:

  static std::string modelName;

  int state;

  unsigned int age;

  unsigned char *data;
  unsigned char *decodedData;

  unsigned char *bodySpecification;
  float fitness;

  BaseReplicator* child;

  BaseReplicator();
  ~BaseReplicator();

  virtual void initializeDecoding();
  virtual unsigned char *copyData();
  virtual void decode();

  void update();

};

#endif /* BASEREPLICATOR_H */
