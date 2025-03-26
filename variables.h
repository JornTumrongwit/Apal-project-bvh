/*****************************************************************************/
/* This is the program skeleton from CSE167 by Ravi Ramamoorthi              */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  
#include <vector>
#include <string>
#include<stdexcept>
#include "matvec.h"
#include "objects.h"
#include "bbox.h"

using Allocator = std::pmr::polymorphic_allocator<std::byte>;
#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

#ifdef MAINPROGRAM
int amountinit = 5;
int w = 500, h = 500; // width and height 
float fovy = 90.0; // For field of view
float fovx = 90;
int reflectdepth = 5;
float ambient[4] = { .2, .2, .2, 1 };
float attenuation[3] = { 1, 0, 0 };
std::string outname = "test.png";
#else
EXTERN int amountinit;
EXTERN int w, h; // width and height 
EXTERN float fovy; // For field of view
EXTERN float fovx;
EXTERN int reflectdepth;
EXTERN float ambient[4];
EXTERN float attenuation[3];
EXTERN std::string outname;
#endif

EXTERN myvec3 eyeinit; // Initial eye position, also for resets
EXTERN myvec3 upinit; // Initial up position, also for resets
EXTERN myvec3 center; // Center look at point 
EXTERN myvec3 ucam;
EXTERN myvec3 vcam;
EXTERN myvec3 wcam;
EXTERN float tany;
EXTERN float tanx;

// Lighting parameter array, similar to that in the fragment shader
EXTERN int numused;                     // How many lights are used 
EXTERN std::vector<myvec4> lightposn; // Light Positions
EXTERN std::vector<myvec3> lightcolor; // Light Colors

// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN float diffuse[3];
EXTERN float specular[3];
EXTERN float emission[3];
EXTERN float shininess;

// For multiple objects, read from a file.  
EXTERN int numspheres;
EXTERN int numtriangles;

EXTERN std::vector<Sphere> SphereList;
EXTERN std::vector<Triangle> TriangleList;
EXTERN std::vector<Object*> ObjectList;
EXTERN std::vector<myvec4> vertices;
EXTERN BBox* BoundingBox;

EXTERN std::vector<CBB> compactBBoxes;

EXTERN int maxPrims;