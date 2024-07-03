#ifndef plot_h
#define plot_h
#include "defines.h"
#include "node.h"
#include <vector>
using namespace std;
class Plot{
 public:
  static void plot2d(vector<SP<Node>> aNodes, int aWidth, float aXmin=0, float aXmax=1, float aYmin=0, float aYmax=1);
};
#endif
