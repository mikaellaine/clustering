#ifndef clustering_h
#define clustering_h
#include "defines.h"
#include "node.h"
#include <vector>
#include <future>
using namespace std;
class Clustering{
 protected:
  vector<int> mDistHistogram;
  SP<Dataset> mData;
  std::mutex mMutexNodeSet;//for protecting setting values in nodes
  int mCores;
  int mXmin=0;
  int mXmax=0;
  int mYmin=0;
  int mYmax=0;
  
 public:
  Clustering();
  void setNeighbors( SP<Node> A, SP<Node> B );
  void setData(SP<Dataset> aData);
  void analyseDistances(int aBuckets, bool aPrint=true, int aPrintRange=20);
  // used by DBSCAN clustering to set initial neighborhoods
  void calculateDistances( float aEpsilon, int aStart, int aEnd );
  void cluster_DBSCAN(float aEpsilon, size_t aMinPts);
  void cluster_DBSCAN_add_near(SP<Node> aNode, SP<Node> aNear);
  void cluster_Kmedian(float aEpsilon, int aMinPts, int aDesiredClustersCount);
  void plot();
};
#endif

