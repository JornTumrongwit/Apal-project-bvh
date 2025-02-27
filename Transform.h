// Transform header file to define the interface. 
// The class is all static for simplicity
// You need to implement left, up and lookAt
// Rotate is a helper function

// Include the helper glm library, including matrix transform extensions

#include "matvec.h"

// glm provides vector, matrix classes like glsl
// Typedefs to make code more readable 
const float pi = 3.14159265 ; // For portability across platforms


class Transform  
{
public:
	Transform();
	virtual ~Transform();
    static mymat3 rotate(const float degrees, const myvec3& axis) ;
    static mymat4 scale(const float &sx, const float &sy, const float &sz) ; 
    static mymat4 translate(const float &tx, const float &ty, const float &tz);
    static myvec3 upvector(const myvec3 &up, const myvec3 &zvec) ; 
};

