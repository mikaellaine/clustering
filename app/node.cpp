#include "node.h"
#include "util.h"
#include <cmath>
#include <fstream> 
#include <iostream> 
#include <string>

void Node::addNeighbor(SP<Node> aNeighbor){
  mNeighbors.push_back(aNeighbor);
}

NodeFloatvec::NodeFloatvec(vector<float> aVals){
  mVal=aVals;
}

float NodeFloatvec::dist(SP<Node> aOther){
  float sum=0.0f;
  float d=0.0f;
  SP<NodeFloatvec> o=std::dynamic_pointer_cast<NodeFloatvec>(aOther);
  if(o){
    for(unsigned int i = 0; i < mVal.size(); ++i){
      d=mVal[i] - o->mVal[i];
      sum += d*d;
    }
  }
  return sqrt(sum);
}

SP<Dataset> NodeFloatvec::read(string aFilePath){
  SP<Dataset> set=MS<Dataset>();
  printf("\n NodeFloatvec reading from file %s", aFilePath.c_str());
  std::ifstream infile(aFilePath);
  if (!infile.is_open()) { cerr << "\nCan't open file "<< aFilePath; return set; }
  SP<NodeFloatvec> node;
  string line;
  int offs=0;
  int newoffs=0;
  string v="";
  int dim=-1;
  vector<float> vals;
  while (getline(infile, line)) {
    offs=newoffs=0;
    vals.clear();
    while(newoffs != -1){
      v=Util::getNextStr(line, ",", offs, newoffs);
      if(v.length() > 0){
        vals.push_back(std::stof(v));
        offs=newoffs+1;
      }
    }
    node=MS<NodeFloatvec>(vals);
    // One line complete
    if(dim == -1){dim=vals.size(); set->mDim=vals.size();}
    set->add(dynamic_pointer_cast<Node>(node));
  }
  cout << " -> completed read of " << set->size() << " tensors of " << set->mDim << " size." << endl;
  infile.close();
  return set;
}
