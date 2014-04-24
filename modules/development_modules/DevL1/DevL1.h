#include "BaseDevMechanism.h"
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

class DevL1 : public BaseDevMechanism
{
public:

  void printDecoder(ofstream &stream);

  DevL1(config &args);
  ~DevL1();

 private:

  unsigned short type;

  unsigned int initProductionRuleSize;
  unsigned int maxWorkingDataSize;

  unsigned short workingDataFrame;
  vector<vector<unsigned short> > workingData;

  vector<unsigned short> productionRuleSpecification; // for use by the non-loopy only
  map<triple, vector<unsigned short> > productionRules;
  vector<vector<unsigned short> > partPunctuationMarks;
  vector<vector<unsigned short> > rulePunctuationMarks;

  bool changedOnLastPass;

  void newDecoder(config&);
  void initializeDecoding(config&);

  void buildProductionRules(config&, vector<unsigned short>::iterator, vector<unsigned short>::iterator, map<triple, vector<unsigned short> >&);
  void interpret(config&, vector<unsigned short>);
  void decode(config&);

};
