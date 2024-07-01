#include "node.h"
#include "util.h"
#include <cmath>
#include <fstream> 
#include <iostream> 
#include <string>

void Node::addNeighbor(SP<Node> aNeighbor){
  mNeighbors.push_back(aNeighbor);
}

float NodeFloatvec::dist(SP<Node> aOther){
  float sum=0.0f;
  float d=0.0f;
  SP<NodeFloatvec> o=dynamic_cast<NodeFloatvec>(aOther);
  for(int i = 0; i < mDim; ++i){
    d=mVal[i]-o->mVal[i];
    sum += d*d;
  }
  return sqrt(sum);
}

void NodeFloatvec::read(string aFilePath){
  ifstream infile(aFilePath); 
    if (!infile.is_open()) { cerr << "\nCan't open file "<< aFilePath; return; } 
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
      // One line complete
      if(mDim == -1){mDim=vals.size();}
      mVal.push_back(vals);
    }
    cout << "Read " << vals.size() << " tensors" << endl;
    infile.close(); 
}
