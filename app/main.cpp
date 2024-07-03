#include <iostream>
#include "clustering.h"

int main(int ac, char **aa){
  cout<<"main";
  SP<Dataset> set = NodeFloatvec::read("/data/misc/clustering/test_moons.csv");
  Clustering c;
  c.setData(set);
  printf("\nanalyseDistances");fflush(stdout);
  c.analyseDistances(100, true);
  c.cluster_DBSCAN(0.12, 5 );
  c.plot();
}
