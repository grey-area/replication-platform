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

class BaseDevMechanism
{

public:

  static std::string devName;
  static int alphabetSize;
  static int initialDataSize;
  static int minDataSize;
  static int maxDataSize;
  static void setArgs(config&);

  int id;
  int state;
  unsigned int age;
  unsigned int gestationTime;
  unsigned int printCount; 
  int parentID;

  // `fitness' is used to determine the metabolism of the organism.
  // Use `score' to track performance in a way that doesn't affect metabolism.
  float fitness;
  float score;

  // The genome of the organism
  std::vector<unsigned short> data;
  // The body specification of the organism
  std::vector<unsigned short> bodySpecification;

  // The organism's child
  BaseDevMechanism* child;

  virtual std::vector<unsigned short> copyData(config&);
  virtual void mutateData(config&, vector<unsigned short>&); // *
  // For creating new random organism
  void initializeOrphan(config&);
  // For printing the state of the organism
  void print(ofstream&);
  void update(config&,globalVars&);

  BaseDevMechanism(config&);
  virtual ~BaseDevMechanism();

 private:

  // ** = override.
  // * = optionally override.

  virtual void initializeOrphanData(config&);    // *
  virtual void initializeOrphanDecoder(config&); // **

  virtual void printBodySpecification(ofstream&);     // *
  virtual void printData(ofstream&);     // *
  virtual void printDecoder(ofstream&);  // **

  virtual void initializeDecoding(config&);              // **
  virtual void decode(config&);     // **

};

#endif /* BASEREPLICATOR_H */
