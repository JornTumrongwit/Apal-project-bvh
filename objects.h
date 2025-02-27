#pragma once
#include "matvec.h"
typedef unsigned char BYTE;

enum SHAPE { NONE, TRIANGLE, SPHERE };

//bounding box
struct BBox{
	myvec3 leftCorner;
	myvec3 rightCorner;
};

//General objects (Sphere and triangles)

class Object {       // The class
public:             // Access specifier
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float emission[3];
	float shininess;
	Object() = default;
	//BBox BoundingBox();
	virtual bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
		return false;
	};
	virtual bool blockCheck(float closest, myvec3 raydir, myvec3 position){
		return false;
	}
};

class Sphere : public Object {
public:
	myvec3 position;
	float radius;
	mymat4 transform;
	mymat4 inv_transform;

	Sphere() = default;
	bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position) override;
	bool blockCheck(float closest, myvec3 raydir, myvec3 position) override;
};

class Triangle : public Object {
public:
	myvec4 vertA;
	myvec4 vertB;
	myvec4 vertC;
	myvec3 normal;

	Triangle() = default;
	bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position) override;
	bool blockCheck(float closest, myvec3 raydir, myvec3 position) override;
};