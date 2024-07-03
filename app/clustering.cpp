
#include <stdio.h>
#include <limits>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>
#include <map>

#include "clustering.h"
#include "util.h"
#include "plot.h"
using namespace std;

Clustering::Clustering(){
  mCores = std::thread::hardware_concurrency();
  if(mCores == 0){mCores=2;}
}

void Clustering::setData(SP<Dataset> aData){
  mData=aData;
  mXmin=10000000;
  mXmax=-1000000;
  mYmin=10000000;
  mYmax=-1000000;
}

// Create histogram of distances
void Clustering::analyseDistances(int aBuckets, bool aPrint, int aPrintRange){
  if(!mData){return;}
  int sampleSize;
  int x;
  int allCount = mData->size();
  vector<SP<Node>> sample;
  vector<float> dists;
  double distsum=0.0;
  double distcnt=0;
  float avg_dist;
  float std_dev;
  float d;
  
  if( allCount > 1000000 )   { sampleSize=10000; }
  else if(allCount > 100000) { sampleSize=allCount / 100; }
  else                       { sampleSize=allCount / 10; }
  int sinc=allCount/sampleSize-1;
  int si=0;
  for(int i = 0; i < sampleSize; ++i){ // Populate sample vector
    si += rand()%sinc;
    sample.push_back(mData->get(si));
  }
  printf("\ncalculate dists for all %d (%zu from %zu)", sampleSize, sample.size(), mData->size());ff;
  // Calculate distances
  distsum=0.0; distcnt=0;
  for(int i = 0; i < sampleSize; ++i){
    for(int j = i+1; j < sampleSize; ++j){
      d=sample[i]->dist(sample[j]);
      distsum += d; distcnt += 1.0;
      dists.push_back( sample[i]->dist(sample[j]) );
    }
  }
  avg_dist=(float)(distsum / distcnt);

  // Calculate std dev
  distsum=0.0; distcnt=0;
  float cdiff;
  int maxcnt=0;
  float maxdist=0;
  float mindist=std::numeric_limits<float>::max();  
  for(int i = 0; i < sampleSize; ++i){
    d=dists[i];
    cdiff = pow(avg_dist - d, 2.0);
    distsum += cdiff; ++distcnt;
    if(d < mindist){mindist=d;}
    if(d > maxdist){maxdist=d;}
  }
  std_dev=(float)(distsum / distcnt);
  printf("\navg_dist: %g std_dev: %g", avg_dist, std_dev);ff;
  std::sort(dists.begin(), dists.end());
  mDistHistogram.clear(); for(int i = 0; i < aBuckets; ++i){ mDistHistogram.push_back(0); }
  float bucket_inc = maxdist / aBuckets;
  for(unsigned int i = 0; i < dists.size(); ++i ){
    d=dists[i];
    for(int bi=1; bi <= aBuckets; ++bi){
      if( d < bi * bucket_inc ){ x = ++mDistHistogram[bi-1]; if( x > maxcnt ){ maxcnt = x; } break; }
    }
  }
  
  // Now print the histogram, if requested
  if( aPrint ){
    float scale=aPrintRange;
    int perscale=maxcnt / scale; perscale=max(1,perscale);
    for( int d = scale; d >= 0; --d ){
      int limit=d*perscale;
      printf("\n");
      for( int i = 0; i < aBuckets; ++i ){
        if(mDistHistogram[i] >= limit) {printf("#");}
        else                           {printf(" ");}
      }
    }
    printf("\n%g .. %g .. %g\n", mindist, avg_dist, maxdist);
    ff;
  }
}

void Clustering::setNeighbors( SP<Node> A, SP<Node> B ){
  std::lock_guard<std::mutex> lock(mMutexNodeSet);
  if(A.get() != B.get()){
    A->addNeighbor(B);
    B->addNeighbor(A);
  }
    if(A->x() < mXmin){mXmin=A->x();}
    if(A->x() > mXmax){mXmax=A->x();}
    if(A->y() < mYmin){mYmin=A->y();}
    if(A->y() > mYmax){mYmax=A->y();}
}

/**
 *@param aEpsilon - neighbor maximum distance (for e.g. DBSCAN)
 *@param aStart
 *@param aEnd
 */
void Clustering::calculateDistances( float aEpsilon, int aStart, int aEnd ){
  float cdist=0;
  SP<Node> cnode, onode;
  for(int i = aStart; i < aEnd; ++i){
    for(int j = i+1; j < aEnd; ++j){
      cnode=mData->get(i);
      onode=mData->get(j);
      cdist = cnode->dist(onode);
      if( cdist < aEpsilon ){
        setNeighbors(cnode, onode);
      }
    }
  }
}
// Add neighbors of aNear, and recursively their neighbors to 'aNode'
void Clustering::cluster_DBSCAN_add_near(SP<Node> aNode, SP<Node> aNear){
  int len=aNear->near.size();
  int lbl=aNode->label;
  SP<Node> nei;
  printf("cluster_add_near %d", lbl);ff;
  for( size_t ni = len-1; ni >= 0; --ni ){
    nei=aNear->near[ni];
    if( nei->label > 0 || nei.get() == aNode.get() ){ continue; }
    nei->label = lbl;
    //aNode->near.push_back(nei);
    //aNear->near.erase(aNear->near.begin() + ni);
    cluster_DBSCAN_add_near(aNode, nei);
  }
  printf("\ncluster_add_near <--");ff;
}
/**
 *
 *@param aEpsilon - max dist of points in initial cluster
 *@param aMinPts - min number of points in initial cluster
 */
void Clustering::cluster_DBSCAN( float aEpsilon, size_t aMinPts ){
  int len = mData->size();
  int pert=len/mCores;
  int st,en;
  // Collect neighborhoods for all nodes. This takes a long time
  printf("\nClustering::cluster_DBSCAN calculating all distances using %d threads\n", mCores);
  vector<std::future<void>> tasks;
  for(int i = 0; i < len; i+=pert){
    st=i;
    en=min(len,i+pert);
    tasks.push_back(std::async(std::launch::async, &Clustering::calculateDistances, this, aEpsilon, st, en));
  }
  for(unsigned int i = 0; i < tasks.size(); ++i){tasks[i].wait(); printf("."); ff; }
  tasks.clear();
  int cid=0;
  printf("\nClustering::cluster_DBSCAN cluster");ff;
  for( int i = 0; i < len; ++i ){
    SP<Node> node=mData->get(i);
    if( node->label > 0){ continue; }
    if( node->near.size() > aMinPts ){ 
      node->label = ++cid;
      cluster_DBSCAN_add_near(node, node);
    }
  }
}

/**
 *
 *
 *@param aEpsilon - distance for initial clustering
 *@param aMinPts - minimum number of nodes in an initial cluster
 */
void Clustering::cluster_Kmedian(float aEpsilon, int aMinPts, int aDesiredClustersCount){
  int len=mData->size();
  map<int, SP<Node>> neighbors;// nodes with the biggest neighborhood
  vector<SP<Node>> cluster_centroids;
  int st, en, n=mData->size(), pert=len/mCores;
  vector<std::future<void>> tasks;
  for(int i = 0; i < n; i+=pert){
    st=i;
    en=min(n,i+pert);
    tasks.push_back(std::async(std::launch::async, &Clustering::calculateDistances, this, aEpsilon, st, en));
  }
  for(unsigned int i = 0; i < tasks.size(); ++i){tasks[i].wait();}
  tasks.clear();
  for(size_t i = 0; i < mData->size(); ++i){
    mData->get(i)->label=0;
    if(mData->get(i)->nSize() > aMinPts ){
      neighbors[mData->get(i)->nSize()] = mData->get(i);
    }
  }
  int cnt=0;
  for(auto ns=neighbors.rbegin(); ns != neighbors.rend(); ++ns){
    if(cnt < aDesiredClustersCount){
      cluster_centroids.push_back( ns->second );
    }
  }
}
void Clustering::plot(){
  Plot::plot2d(mData->mNodes, 20, mXmin, mXmax, mYmin, mYmax);
}
