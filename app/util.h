#ifndef clustering_util_h
#define clustering_util_h
#include <string>

using namespace std;
class Util{
 public:
  static string getNextStr(string& aInput, string aDelim, int aOffs, int& aEnd);
};
#endif
