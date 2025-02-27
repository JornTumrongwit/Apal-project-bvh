#pragma once
#include "matvec.h"
typedef unsigned char BYTE;

enum SHAPE { NONE, TRIANGLE, SPHERE };

//General objects (Sphere and triangles)

class Object {       // The class
public:             // Access specifier
	struct BBox{
		myvec3 leftCorner;
		myvec3 rightCorner;
	};

	float ambient[3];
	float diffuse[3];
	float specular[3];
	float emission[3];
	float shininess;
	Object() = default;
	BBox BoundingBox();
	bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position);
};

class Sphere : public Object {
public:
	myvec3 position;
	float radius;
	mymat4 transform;
	mymat4 inv_transform;

	Sphere() = default;
};

class Triangle : public Object {
public:
	myvec4 vertA;
	myvec4 vertB;
	myvec4 vertC;
	myvec3 normal;

	Triangle() = default;
};