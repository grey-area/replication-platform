#ifndef BASEREPLICATOR_H
#define BASEREPLICATOR_H

class BaseReplicator
{
public:
  BaseReplicator();
  ~BaseReplicator();
  virtual BaseReplicator* update();
};

#endif /* BASEREPLICATOR_H */
