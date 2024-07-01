string Util::getNextStr(string& aInput, string aDelim, int aOffs, int& aEnd){
  aEnd = aInput.find(aDelim, aOffs);
  if(aEnd != -1){ return aInput.substr(aOffs, (i-aEnd)); }
  return "";
}
