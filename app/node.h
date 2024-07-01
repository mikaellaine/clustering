#ifndef clustering_node_h
#define clustering_node_h
#include "defines.h"
#include <vector>
using namespace std;
class Node{
protected:
  
 public:
  vector<SP<Node>> mNeighbors;
  int label=0;// -1 = noise
  virtual float dist(SP<Node> aOther)=0;
  void addNeighbor(SP<Node> aNeighbor);
};

class NodeFloatvec{
protected:
  vector<vector<float>> mVal;
  int mDim;// dimension of vectors
public:
  virtual float dist(SP<Node> aOther);
  virtual void read(string aFilePath);
};

class Dataset{
public:
  vector<SP<Node>> mNodes;
  size_t size(){ return mNodes.size(); }
  void add(SP<Node> aNode){ mNodes.push_back(aNode); }
  SP<Node> get(int aIndex){ return mNodes[aIndex]; }
};


#endif