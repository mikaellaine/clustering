#include "node.h"
#include "util.h"
#include <cmath>
#include <fstream> 
#include <iostream> 
#include <string>

void Node::addNeighbor(SP<Node> aNeighbor){
  mNeighbors.push_back(aNeighbor);
}

virtual float NodeFloatvec::dist(SP<Node> aOther){
  float sum;
  float d;
  for(int i = 0; i < mDim; ++i){
    d=mVal[i]-aOther->mVal[i];
    sum += d*d;
  }
  return sqrt(sum);
}

virtual void NodeFloatvec::read(string aFilePath){
    ifstream infile(aFilePath); 
    if (!infile.is_open()) { cerr << "\nCan't open file "<< aFilePath; return; } 
    string line;
    int offs=0;
    int newoffs=0;
    while (getline(infile, line)) {
      Util::getNextStr(line, ",", offs, newoffs);
      cout << line << endl; // Print the current line 
    }
    infile.close(); 
}
