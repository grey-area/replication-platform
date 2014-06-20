#include "BaseOrganism.h"
#include <map>

class triple {
public:
    unsigned short x;
    unsigned short y;
    unsigned short z;
    //bool operator<(const triple &other) const {
    //    return (x < other.x && y < other.y && z < other.z);
    //}

    bool operator==(const triple &o) const {
        return x == o.x && y == o.y && z==o.z;
    }

    bool operator<(const triple &o) const {
      return x < o.x || (x == o.x && y < o.y) || (x ==o.x && y == o.y && z < o.z);
    }
};

#define LOOPY     0
#define UNLOOPY   1
#define GA        2

class OrgL1 : public BaseOrganism
{
public:

  void printDevelopmentProcess(ofstream &stream);

  OrgL1(config &args, globalVars &global);
  ~OrgL1();

 private:

  unsigned short type;

  unsigned int initProductionRuleSize;
  unsigned int maxWorkingGenomeSize;

  unsigned short workingGenomeFrame;
  vector<vector<unsigned short> > workingGenome;

  vector<unsigned short> productionRuleSpecification; // for use by the non-loopy only
  map<triple, vector<unsigned short> > productionRules;
  vector<vector<unsigned short> > partPunctuationMarks;
  vector<vector<unsigned short> > rulePunctuationMarks;

  bool changedOnLastPass;

  void initializeOrphanDevelopmentProcess(config&);
  void initializeDevelopmentProcess(config&);

  void buildProductionRules(config&, vector<unsigned short>::iterator, vector<unsigned short>::iterator, map<triple, vector<unsigned short> >&);
  void interpret(config&, vector<unsigned short>);
  void updateDevelopment(config&);

};
