// Readfile definitions 
#include "matvec.h"
#include <sstream>
#include <stack>
void matransform (std::stack<mymat4> &transfstack, float * values) ;
void rightmultiply (const mymat4 & M, std::stack<mymat4> &transfstack) ;
bool readvals (std::stringstream &s, const int numvals, float* values) ;
bool readind(std::stringstream& s, const int numvals, int* val);
void readfile (const char * filename) ;
