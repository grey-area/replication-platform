#include "BaseDevMechanism.h"

class DevGA : public BaseDevMechanism
{
 public:
  DevGA(config&);
  ~DevGA();

  // Optional
  //void mutateData(config&, vector<unsigned short>&);

 private:
  void printDecoder(ofstream&);
  void initializeOrphanDecoder(config&);
  void initializeDecoding(config&);
  void decode(config&);

  /* Mechanism-specific variables. Suggestions:
     - Somewhere to store a working copy of the decoded data.
     - Variable for initial size of orphan decoders.
     - Variable for maximum size of decoded data.
   */

  // Optional
  //void initializeOrphanData(config&);
  //void printData(ofstream&);
};
