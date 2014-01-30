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
  static void setArgs(std::string,int,int,int,int);

  int id;
  int state;
  unsigned int age;
  unsigned int gestationTime;

  std::vector<unsigned short> data;
  std::vector<unsigned short> bodySpecification;
  float fitness;

  BaseReplicator* child;

  BaseReplicator(int);
  virtual ~BaseReplicator();

  void printData(std::vector<unsigned short>);

  // For creating new random entity
  void newEntity(int);

  void update(int);

 private:

  // Called by newEntity
  virtual void newData(int);    // *
  virtual void newDecoder(int); // **

  virtual void initializeDecoding(int);              // **
  virtual std::vector<unsigned short> copyData(int); // *
  virtual void decode(int);     // **

  // ** = override. * = optionally override.

};

#endif /* BASEREPLICATOR_H */
