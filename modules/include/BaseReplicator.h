#include <string>
#include <vector>
#include "config.h"
#include <fstream>

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
  static void setArgs(config&);

  int id;
  int state;
  unsigned int age;
  unsigned int gestationTime;
  unsigned int identical; // 0 = not identical to parent, 1 = is identical, 2 = there was a mutation, so we're not going to check
  unsigned int printCount;
  int parentID;

  std::vector<unsigned short> data;
  std::vector<unsigned short> bodySpecification;

  float fitness;
  float score;

  BaseReplicator* child;

  BaseReplicator(config&);
  virtual ~BaseReplicator();

  // For creating new random entity
  void newEntity(config&);

  // For printing decoder and data
  void print(ofstream&);

  virtual std::vector<unsigned short> copyData(config&); // *
  void update(config&);

 private:

  // Called by newEntity
  virtual void newData(config&);    // *
  virtual void newDecoder(config&); // **

  // Called by print
  virtual void printDecoder(ofstream&);  // **
  virtual void printData(ofstream&);     // *

  virtual void initializeDecoding(config&);              // **
  virtual void decode(config&);     // **

  // ** = override. * = optionally override.

};

#endif /* BASEREPLICATOR_H */
