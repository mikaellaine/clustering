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
 public:
  Clustering();
  void setNeighbors( SP<Node> A, SP<Node> B );
  void setData(SP<Dataset> aData);
  void analyseDistances(int aBuckets, bool aPrint=true);
  // used by DBSCAN clustering to set initial neighborhoods
  void calculateDistances( float aEpsilon, int aStart, int aEnd );
  void cluster_DBSCAN(float aEpsilon, size_t aMinPts);
  void cluster_Kmedian(float aEpsilon, int aMinPts, int aDesiredClustersCount);
};
#endif

