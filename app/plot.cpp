#include "plot.h"
#include "util.h"
#include <stdio.h>
#include <iostream>
#include <climits>

void Plot::plot2d(vector<SP<Node>> aNodes, int aWidth, float aXmin, float aXmax, float aYmin, float aYmax){
  vector<vector<int>> bmap;
  float xspan=aXmax-aXmin;
  float yspan=aYmax-aYmin;
  float xinc=xspan / aWidth;
  float yinc=yspan / aWidth;
  for(int y=0; y < aWidth; ++y){
    vector<int> line;
    for(int x=0; x < aWidth; ++x){ line.push_back(-2); }
    bmap.push_back(line);
  }
  float xmax=-1000000;
  float xmin=1000000;
  float ymax=-1000000;
  float ymin=1000000;
  for(size_t i = 0; i < aNodes.size(); ++i){
    SP<Node> n=aNodes[i];
    float x=n->x();
    float y=n->y();
    if( x < xmin ){xmin=x;}
    if( x > xmax ){xmax=x;}
    if( y < ymin ){ymin=y;}
    if( y > ymax ){ymax=y;}
  }
  xspan=xmax-xmin;
  yspan=ymax-ymin;
  xinc=xspan / aWidth;
  yinc=yspan / aWidth;  
  printf("\nX: %g -> %g -> %g  Y: %g -> %g -> %g\n", xmin, xinc, xmax, ymin, yinc, ymax);
  for(size_t i = 0; i < aNodes.size(); ++i){
    SP<Node> n=aNodes[i];
    float x=n->x();
    float y=n->y();
    if( x < xmin ){continue;}
    if( y < ymin ){continue;}
    int xi=0,yi=0;
    for(float sx=xmin; sx <= xmax; sx += xinc){ if( x < sx ){ break; } ++xi; }
    for(float sy=ymin; sy <= ymax; sy += yinc){ if( y < sy ){ break; } ++yi; }
    if( yi >= aWidth-1 ){yi=aWidth-1;}
    if( xi >= aWidth-1 ){xi=aWidth-1;}
    bmap[yi][xi]=n->label;
  }
  for(int y = 0; y < aWidth; ++y){
    cout<<endl;
    for(int x = 0; x < aWidth; ++x){
      if(bmap[y][x] < 0){
        cout<<"   ";
      }else{
        string nums=std::to_string(bmap[y][x]);
        nums=Util::paddString(nums, 3);
        cout<<nums;
      }
    }
  }
  printf("\nAll done.\n");
}
