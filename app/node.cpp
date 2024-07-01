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
  for(int i = 0; i < mDim; ++i){
    d=mVal[i] - o->mVal[i];
    sum += d*d;
  }
  return sqrt(sum);
}

SP<Dataset> NodeFloatvec::read(string aFilePath){
  SP<Dataset> set;
  ifstream infile(aFilePath);
    if (!infile.is_open()) { cerr << "\nCan't open file "<< aFilePath; return set; }
    SP<NodeFloatvec> node;
    string line;
    int offs=0;
    int newoffs=0;
    string v="";
    mVal.clear();
    mDim=-1;
    vector<float> vals;
    while (getline(infile, line)) {
      while(newoffs != -1){
        v=Util::getNextStr(line, ",", offs, newoffs);
        float f=std::stof(v);
        vals.push_back(f);
        cout << line << endl; // Print the current line
        offs=newoffs;
      }
      node=MS<NodeFloatvec>(vals);
      // One line complete
      if(mDim == -1){mDim=vals.size();}
      set->add(dynamic_pointer_cast<Node>(node));
    }
    cout << "Read " << vals.size() << " tensors" << endl;
    infile.close();
    return set;
}
