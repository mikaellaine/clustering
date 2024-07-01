#ifndef clustering_node_h
#define clustering_node_h
#include "defines.h"
#include <vector>
using namespace std;
class Dataset;
class Node{
protected:
 public:
  vector<SP<Node>> mNeighbors;
  int label=0;// -1 = noise
  virtual float dist(SP<Node> aOther)=0;
  void addNeighbor(SP<Node> aNeighbor);
  int nSize(){return mNeighbors.size(); }
};

class NodeFloatvec : public Node{
protected:
  vector<float> mVal;
public:
  NodeFloatvec(vector<float> aVals);
  virtual float dist(SP<Node> aOther);
  static SP<Dataset> read(string aFilePath);
};

class Dataset{
public:
  int mDim;//how many elements per tensor
  vector<SP<Node>> mNodes;
  size_t size(){ return mNodes.size(); }
  void add(SP<Node> aNode){ mNodes.push_back(aNode); }
  SP<Node> get(int aIndex){ return mNodes[aIndex]; }
};


#endif
