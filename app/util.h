#ifndef clustering_util_h
#define clustering_util_h
#include <string>
#include <vector>

using namespace std;
class Util{
 public:
  static string getNextStr(string& aInput, string aDelim, int aOffs, int& aEnd);
  static string printString(vector<string> vals, int aLen);
};
#endif
