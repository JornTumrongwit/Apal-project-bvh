#include "objects.h"
#include "bbox.h"
#include <fstream>

myvec3 raytracer(myvec3 raydir, myvec3 position, float depth);
myvec3 raytrace_timed(myvec3 raydir, myvec3 position, float depth, std::ofstream& statsfile);
myvec3 ComputeLight(myvec3 direction, myvec3 lightcolor, myvec3 normal, myvec3 halfvec, myvec3 mydiffuse, myvec3 myspecular, float myshininess);\
bool blockCheck(float closest, myvec3 raydir, myvec3 position);
void get_color_sph(myvec3 raydir, myvec3 intersect, Sphere* hitsph, myvec3 position, myvec3 getnormal, myvec3& pixels, float depth);
void get_color_tri(myvec3 raydir, myvec3 intersect, Triangle* hittri, myvec3 position, myvec3 viewnormal, myvec3& pixels, float depth);
void get_color(myvec3 raydir, myvec3 mypos, myvec3 intersect, myvec3 eyedirn, myvec3 normal, myvec3 diffuse, myvec3 specular, float shininess, float* ambience, float* emission, myvec3& endcolor, float depth);
myvec3 toVec3(float* inp);
