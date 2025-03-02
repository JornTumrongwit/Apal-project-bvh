#pragma once
#include "matvec.h"
#include <iostream>
typedef unsigned char BYTE;

enum SHAPE { NONE, TRIANGLE, SPHERE };

//General objects (Sphere and triangles)

class Object {       // The abstract class for polygons
public:             // Access specifier
	SHAPE poly_type = NONE;
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float emission[3];
	float shininess;
	Object() = default;

	virtual void Bound(myvec3* bottomleft, myvec3* topright){
		return;
	};

	virtual bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
		std::cout<<"Weird\n";
		return false;
	};
	virtual bool blockCheck(float closest, myvec3 raydir, myvec3 position){
		std::cout<<"Weird\n";
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
	bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position);
	bool blockCheck(float closest, myvec3 raydir, myvec3 position);
	void Bound(myvec3* bottomleft, myvec3* topright);
};

class Triangle : public Object {
public:
	myvec4 vertA;
	myvec4 vertB;
	myvec4 vertC;
	myvec3 normal;

	Triangle() = default;
	bool CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position);
	bool blockCheck(float closest, myvec3 raydir, myvec3 position);
	void Bound(myvec3* bottomleft, myvec3* topright);
};