/******************************************************************************/
/* This is the program skeleton modifed from CSE167 by Ravi Ramamoorthi       */
/* Modified for ray tracing pipelines                                         */
/******************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "./Transform.h"
#include "./raytracer.h"
#include "./bbox.h"
#include "objects.h"
// Main variables in the program.  
#define MAINPROGRAM 
#include "./variables.h" 
#include "./readfile.h" // prototypes for readfile.cpp  
#include "libmorton/morton.h"

using namespace std ;

void saveScreenshot(string fname) {
  maxPrims = 1000;
  ofstream PicFile(fname);
  PicFile<<"P3\n"<<w<<' '<<h<<'\n'<<255<<'\n';
  const int width = w;
  const int height = h;
  std::vector<myvec3> pix;

  pix.resize(w*h);

  // BoundingBox = new SimpleBBox(std::max((int) TriangleList.size()/100, 5));
  // //BBox construct
  // for (Triangle tri: TriangleList){
  //   BoundingBox->unionBounds(tri);
  // }
  // BoundingBox->split();
  // BoundingBox->compact();
  
  BoundingBox = new MortonBBox();
  BoundingBox->compact();

  //construct object vector
  for(int i = 0; i<TriangleList.size(); i++){
    Triangle* tri = &TriangleList[i];
    Object* obj = tri;
    ObjectList.push_back(obj);
  }
  for(int i = 0; i<SphereList.size(); i++){
    Sphere* sph = &SphereList[i];
    Object* obj = sph;
    ObjectList.push_back(obj);
  }
  // for(Triangle tri: TriangleList){
  //   printvec3(tri.centroid());
  //   std::cout<<"\n";
  // }

  std::cout<<"Setup complete, running renders\n";
  #pragma omp parallel for
  for (int j = h-1; j >=0; j--) {
      for (int i = 0; i < w; i++) {
          float alpha = tanx * (i + 0.5 - (w / 2)) / (w / 2);
          float beta = tany * (j + 0.5 - (h / 2)) / (h / 2);
          myvec3 raydir = normalize(alpha * ucam + beta * vcam - wcam);
          myvec3 fragColor = raytracer(raydir, eyeinit, 1);
          //std::cout << "(" << i << ", " << j << ")\n";
          pix[i + w*j].x = fragColor.x;
          pix[i + w*j].y = fragColor.y;
          pix[i + w*j].z = fragColor.z;
      }
  }

  for (int j = h-1; j >=0; j--) {
      for (int i = 0; i < w; i++) {
        int red = static_cast<int>(pix[i + w*j].x * 255.999);
        int green = static_cast<int>(pix[i + w*j].y * 255.999);
        int blue = static_cast<int>(pix[i + w*j].z * 255.999);
        PicFile << red << ' ' << green << ' ' << blue << '\n';
      }
  }
  PicFile.close();
}

int main(int argc, char* argv[]) {

  if (argc < 2) {
    cerr << "Needs scene argument\n"; 
    exit(-1); 
  }
  readfile(argv[1]);

  std::cout<<"File read\n";
  saveScreenshot("../output/out.ppm");
  return 0;
  // myvec3 min = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
  // myvec3 max = myvec3(FLT_MIN, FLT_MIN, FLT_MIN);
  // for(Triangle tri: TriangleList){
  //   tri.Bound();
  //   myvec3 centroid = tri.centroid();
  //   min.x = std::min(min.x, centroid.x);
  //   min.y = std::min(min.y, centroid.y);
  //   min.z = std::min(min.z, centroid.z);

  //   max.x = std::max(max.x, centroid.x);
  //   max.y = std::max(max.y, centroid.y);
  //   max.z = std::max(max.z, centroid.z);
  // }

  // int maxval = 2047;

  // for(Triangle tri: TriangleList){
  //   myvec3 centroid = tri.centroid();
  //   printvec3(centroid);
  //   std::cout<<"\n";
  //   myvec3 normalized = myvec3((((centroid.x - min.x) / (max.x-min.x))* maxval), 
  //                               (((centroid.y - min.y) / (max.y-min.y))* maxval),
  //                               (((centroid.z - min.z) / (max.z-min.z))* maxval));
  //   std::cout<<(int) normalized.x<<" "<<(int) normalized.y<<" "<<(int) normalized.z<<"\n";
  //   int m3d = libmorton::morton3D_32_encode((int) normalized.x, (int) normalized.y, (int) normalized.z);
  //   //int m3d = libmorton::morton3D_32_encode(1, 2, 3);
  //   std::cout << m3d << "\n";
  //   uint_fast16_t x;
  //   uint_fast16_t y;
  //   uint_fast16_t z;
  //   libmorton::morton3D_32_decode(m3d, x, y, z);
  //   myvec3 final_vec = myvec3(((float(x) / maxval) * (max.x-min.x) + min.x), 
  //                             ((float(y) / maxval) * (max.y-min.y) + min.y),
  //                             ((float(z) / maxval) * (max.z-min.z) + min.z));
  //   printvec3(final_vec);
  //   std::cout<<"\n"<<x<<" "<<y<<" "<<z<<"\n";
  // }
}
