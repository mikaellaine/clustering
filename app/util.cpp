#include "util.h"
string Util::getNextStr(string& aInput, string aDelim, int aOffs, int& aEnd){
  aEnd = aInput.find(aDelim, aOffs);
  if(aEnd != -1){ return aInput.substr(aOffs, (aEnd-aOffs)); }
  else if(aOffs < aInput.length()){ return aInput.substr(aOffs);}
  return "";
}
string Util::printString(vector<string> vals, int aLen){
  int tLen=0;
  int inc=1;
  string res="";
  for(size_t i = 0; i < vals.size(); ++i){
    tLen += vals[i].length();
  }
  inc = (aLen - tLen)/vals.size();
  if (inc < 0){inc=1;}
  for(size_t i = 0; i < vals.size(); ++i){
    res += vals[i];
    for(int s=0; s < inc; ++s){ res += " "; }
  }
  return res;
}
