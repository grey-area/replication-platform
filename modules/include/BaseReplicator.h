#ifndef BASEREPLICATOR_H
#define BASEREPLICATOR_H

class BaseReplicator
{
public:
  int fitness;
  int age;
  int *bodySpecification;
  BaseReplicator* child;

  BaseReplicator();
  ~BaseReplicator();
  virtual void update();
  void baseUpdate();
};

#endif /* BASEREPLICATOR_H */
