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

using namespace std ; 

void saveScreenshot(string fname) {
  ofstream PicFile(fname);
  PicFile<<"P3\n"<<w<<' '<<h<<'\n'<<255<<'\n';
  const int width = w;
  const int height = h;
  std::vector<myvec3> pix;

  pix.resize(w*h);

  //BBox check
  for (Object* obj: ObjectList){
    BBox* sBB = new SimpleBBox(obj);
    sBB->printcheck();
  }
  
  #pragma omp parallel for
  for (int j = h-1; j >=0; j--) {
      for (int i = 0; i < w; i++) {
          float alpha = tanx * (i + 0.5 - (w / 2)) / (w / 2);
          float beta = tany * (j + 0.5 - (h / 2)) / (h / 2);
          //std::cout << "(" << i << ", " << j << ")\n";
          myvec3 raydir = normalize(alpha * ucam + beta * vcam - wcam);
          myvec3 fragColor = raytracer(raydir, eyeinit, 1);
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
  saveScreenshot("output/out.ppm");
  return 0;
}
