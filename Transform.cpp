// Transform.cpp: implementation of the Transform class.

#include "Transform.h"
#include <iostream>
#include <cmath>

// Helper rotation function.  Please implement this.  
mymat3 Transform::rotate(const float degrees, const myvec3& axis) 
{
    // DONE
    myvec3 normaxis = normalize(axis);
    float radians = degrees * pi / 180;
    return mymat3(1.0f) * cos(radians) +
        mymat3(normaxis.x * normaxis, normaxis.y * normaxis, normaxis.z * normaxis) * (1 - cos(radians)) +
        mymat3(myvec3(0, -normaxis.z, normaxis.y), myvec3(normaxis.z, 0, -normaxis.x), myvec3(-normaxis.y, normaxis.x, 0)) * sin(radians);
}

mymat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    // DONE
    return mymat4(sx, 0, 0, 0,
                0, sy, 0, 0,
                0, 0, sz, 0,
                0, 0, 0, 1);
}

mymat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    // DONE
    return mymat4(1, 0, 0, tx,
                0, 1, 0, ty,
                0, 0, 1, tz,
                0, 0, 0, 1);
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

myvec3 Transform::upvector(const myvec3 &up, const myvec3 & zvec) 
{
    myvec3 x = cross(up,zvec); 
    myvec3 y = cross(zvec,x); 
    myvec3 ret = normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
