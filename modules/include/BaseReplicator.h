#include <string>
#include <vector>

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
  static int alphabetSize;
  static int initialDataSize;
  static int minDataSize;
  static int maxDataSize;

  int id;
  int state;
  unsigned int age;

  std::vector<unsigned short> data;
  std::vector<unsigned short> bodySpecification;
  float fitness;

  BaseReplicator* child;

  BaseReplicator();
  ~BaseReplicator();

  void printData(std::vector<unsigned short>);

  // For creating new random entity
  virtual void newData();    // *
  virtual void newDecoder(); // **
  void newEntity();

  virtual void initializeDecoding();              // **
  virtual std::vector<unsigned short> copyData(); // *
  virtual void decode();     // **

  // ** = override. * = optionally override.

  void update();

};

#endif /* BASEREPLICATOR_H */
