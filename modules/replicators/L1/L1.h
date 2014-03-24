#include "BaseReplicator.h"
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

class L1 : public BaseReplicator
{
public:
  L1(config &args);
  ~L1();

 private:

  unsigned short alphabet;
  unsigned int initWorkingDataSize;
  unsigned int maxWorkingDataSize;

  unsigned short workingDataFrame;
  vector<vector<unsigned short> > workingData;

  map<triple, vector<unsigned short> > productionRules;
  vector<vector<unsigned short> > partPunctuationMarks;
  vector<vector<unsigned short> > rulePunctuationMarks;

  bool changedOnLastPass;

  void printDecoder();

  void newDecoder(config&);
  void initializeDecoding(config&);

  void interpret(config&, vector<unsigned short>);
  void decode(config&);

};
