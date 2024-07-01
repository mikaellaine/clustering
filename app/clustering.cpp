#include <stdio.h>
#include <limits>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>
#include <map>

#include "clustering.h"
using namespace std;

Clustering::Clustering(){
  mCores = std::thread::hardware_concurrency();
  if(mCores == 0){mCores=2;}
}

void Clustering::setData(SP<Dataset> aData){
  mData=aData;
}

// Create histogram of distances
void Clustering::analyseDistances(vector<SP<Node>> aNodes, int aBuckets, bool aPrint){
  int sampleSize;
  int allCount = aNodes.size();
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
  for(int i = 0; i < sampleSize; ++i){ // Populate sample vector
    int si = rand()%allCount;
    sample.push_back(aNodes[si]);
  }
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
  for(int i = 0; i < sampleSize; ++i){
    cdiff = pow(avg_dist - dists[i], 2.0);
    distsum += cdiff; ++distcnt;
  }
  std_dev=(float)(distsum / distcnt);
  std::sort(dists.begin(), dists.end());
  // Make histogram span from avg_dist +- std_dev*3
  mDistHistogram.clear();
  for(int i = 0; i < aBuckets; ++i){ mDistHistogram.push_back(0.0f); }
  
  float bucket_increment = std_dev * 6 / (float)aBuckets;
  unsigned int bucketi=0;
  int cnt = 0;
  float maxdist=0;
  float mindist=std::numeric_limits<float>::min();

  for(unsigned int i = 0; i < dists.size(); ++i ){
    if(dists[i] < bucket_increment*(bucketi + 1)){
      mDistHistogram[bucketi] += dists[i]; ++cnt;
    }else{
      mDistHistogram[bucketi] /= cnt; 
      if( mDistHistogram[bucketi] > maxdist ){ maxdist = mDistHistogram[bucketi]; }
      if( mDistHistogram[bucketi] < mindist ){ mindist = mDistHistogram[bucketi]; }
      ++bucketi;
    }
    if( bucketi >= mDistHistogram.size() ){ break; }
  }
  // Now print the histogram, if requested
  if( aPrint ){
    float scale=10;
    float distinc=abs((maxdist-mindist)/scale);
    for( int d = maxdist; d >= mindist; d -= distinc ){
      for( int i = 0; i < aBuckets; ++i ){
        if(mDistHistogram[i] >= d){printf("#");}else{printf(" ");}
      }
    }
  }
}

void Clustering::setNeighbors( SP<Node> A, SP<Node> B ){
  std::lock_guard<std::mutex> lock(mMutexNodeSet);
  A->addNeighbor(B);
  B->addNeighbor(A);
}

/**
 *@param aEpsilon - neighbor maximum distance (for e.g. DBSCAN)
 *@param aStart
 *@param aEnd
 */
void Clustering::calculateDistances( float aEpsilon, int aStart, int aEnd ){
  printf("Clustering::calculateDistances(%d.. %d)", aStart, aEnd);fflush(stdout);
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
/**
 *
 *@param aEpsilon - max dist of points in initial cluster
 *@param aMinPts - min number of points in initial cluster
 */
void Clustering::cluster_DBSCAN( float aEpsilon, int aMinPts ){
  int n = mData->size();
  int pert=n/mCores;
  int st,en;
  // Collect neighborhoods for all nodes. This takes a long time
  vector<std::future<void>> tasks;
  for(int i = 0; i < n; i+=pert){
    st=i;
    en=min(n,i+pert);
    tasks.push_back(std::async(std::launch::async, &Clustering::calculateDistances, this, aEpsilon, st, en));
  }
  for(unsigned int i = 0; i < tasks.size(); ++i){tasks[i].wait();}
  tasks.clear();
  int cid=0;
  // Proceed with clustering
  for( int i = 0; i < n; ++i ){
    SP<Node> node=mData->get(i);
    if(node->label != 0){continue;}
    if( node->mNeighbors.size() < aMinPts ){ node->label=-1; continue; }
    ++cid;
    node->label=cid;
    for( auto nei : node->mNeighbors ){
      if( nei->label == -1 ){ nei->label = cid; }
      if( nei->label != 0 ){ continue; }
      nei->label = cid;
      if( nei->mNeighbors.size() > aMinPts ){
        node->mNeighbors.insert(node->mNeighbors.end(), nei->mNeighbors.begin(), nei->mNeighbors.end());
        // we don't need nei's neighbors anymore
        nei->mNeighbors.clear();
      }
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
  map<int, SP<Node>> neighbors;// nodes with the biggest neighborhood
  vector<SP<Node>> cluster_centroids;
  int st, en, n=mData->size(), pert=n/mCores;
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

int main(int ac, char **aa){
  cout<<"main";
}
