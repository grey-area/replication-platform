#include <string>
#include <vector>
#include "config.h"
#include <fstream>

#ifndef BASEREPLICATOR_H
#define BASEREPLICATOR_H

#define START       0
#define GESTATION   1
#define REPRODUCED  2
#define EMBRYO      9

class BaseOrganism
{

public:

  static std::string organismModuleName;
  static int alphabetSize;
  static int initialGenomeSize;
  static int minGenomeSize;
  static int maxGenomeSize;
  static void setArgs(config&);

  int id;
  int parentID;
  int x;
  int y;
  int species;

  int state;
  unsigned int generationOfBirth;
  unsigned int age;
  unsigned int gestationTime;

  // The environment sets the metabolism based on how the soma interacts with the environment
  float metabolism;
  float parentMetabolism;

  // The genome of the organism
  std::vector<unsigned short> genome;
  // The body specification of the organism
  std::vector<unsigned short> somaSpecification;

  // The organism's child
  BaseOrganism* child;

  virtual std::vector<unsigned short> copyGenome(config&);
  virtual int mutateGenome(config&, vector<unsigned short>&); // *
  void mutateChild(config&, BaseOrganism*);
  // For creating new random organism
  void initializeOrphan(config&);
  // For printing the state of the organism
  void print(ofstream&);
  void update(config&,globalVars&);

  void updateMetabolism(float);

  BaseOrganism(config&, globalVars&);
  virtual ~BaseOrganism();

 private:

  // ** = override.
  // * = optionally override.

  virtual void initializeOrphanGenome(config&);    // *
  virtual void initializeOrphanDevelopmentProcess(config&); // **

  virtual void printSomaSpecification(ofstream&);     // *
  virtual void printGenome(ofstream&);     // *
  virtual void printDevelopmentProcess(ofstream&);  // **

  virtual void initializeDevelopmentProcess(config&);              // **
  virtual void updateDevelopment(config&);     // **

};

#endif /* BASEREPLICATOR_H */
