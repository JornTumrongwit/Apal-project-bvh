#include "objects.h"
#include <cmath>
#include <math.h>
typedef unsigned char BYTE;

//General object functions (Sphere and triangles)
bool Sphere::CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
	/*
	std::clog<<"raydir: ";
	printvec3(*raydir);
	std::clog<<"\n";
	*/
	myvec3 p1 = this->inv_transform * *raydir;
	/*
	std::clog<<"P1: ";
	printvec3(p1);
	std::clog<<"\n";*/
	myvec4 p0 = this->inv_transform * myvec4(position->x, position->y, position->z, 1);
	/*
	std::clog<<"treye: ";
	printvec4(treye);
	std::clog<<"\n";
	*/
	myvec3 P0minusC = myvec3(p0.x, p0.y, p0.z) - this->position;
	float a = dot(p1, p1);
	float b = 2 * dot(p1, P0minusC);
	float c = dot(P0minusC, P0minusC) - this->radius * this->radius;
	float checker = b * b - 4 * a * c;
	//std::clog<<"checker: "<<checker<<"\n";
	if (checker >= 0) {
		//std::clog<<"Has intersect\n";
		float sqrtres = sqrt(checker);
		float distsmall = (-b - sqrtres) / (2 * a);
		//std::clog<<distsmall<<"\n";
		if (distsmall > 0) {
			myvec4 oldinters = p0 + distsmall * p1;
			if (distsmall < closest) {
				myvec4 inters = this->transform * oldinters;
				point_int->x = inters.x;
				point_int->y = inters.y;
				point_int->z = inters.z;
				closest = distsmall;
				myvec3 newnormal = myvec3(oldinters.x, oldinters.y, oldinters.z) - this->position;
				normal.x = newnormal.x;
				normal.y = newnormal.y;
				normal.z = newnormal.z;
				return true;
			}
		}
		else {
			float distbig = (-b + sqrtres) / (2 * a);
			//std::clog<<distbig<<"\n";
			if (distbig > 0) {
				myvec4 oldinters = p0 + distbig * p1;
				myvec4 inters = this->transform * oldinters;
				if (distbig < closest) {
					point_int->x = inters.x;
					point_int->y = inters.y;
					point_int->z = inters.z;
					closest = distbig;
					myvec3 newnormal = myvec3(oldinters.x, oldinters.y, oldinters.z) - this->position;
					normal.x = newnormal.x;
					normal.y = newnormal.y;
					normal.z = newnormal.z;
					return true;
				}
			}
		}
	}
	return false;
}

bool Triangle::CheckIntersect(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
	//std::clog << "called triangle\n";
	//std::clog << "raydir: " << raydir.x << " "<<raydir.y << " "<<raydir.z << "\n";
	float P1DotN = dot(*raydir, this->normal);
	if (P1DotN == 0) return false;
	float t = (dot(this->vertA, this->normal) - dot(*position, this->normal)) / P1DotN;
	if(t>=closest || t<0) return false;
	myvec3 inters = (t * *raydir) + *position;
	/*
	std::clog<<"Inters: ";
	printvec3(inters);
	std::clog<<"\n";
	*/
	myvec3 PminusA = inters - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	myvec3 BminusA = myvec3(this->vertB.x, this->vertB.y, this->vertB.z) - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	myvec3 CminusA = myvec3(this->vertC.x, this->vertC.y, this->vertC.z) - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	float beta = -1;
	float gamma = -1;
	if(BminusA.y == 0 && CminusA.y == 0){
		if(BminusA.x == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.z) - ((PminusA.x) * (BminusA.z) / (BminusA.x));
		float betafromgamma = ((CminusA.x) * (BminusA.z) / (BminusA.x));
		float multiplier = -betafromgamma + (CminusA.z);
		gamma = constantterm / multiplier;
		beta = (PminusA.x - gamma * (CminusA.x)) / (BminusA.x);
	}
	else if(BminusA.x == 0 && CminusA.x == 0){
		if(BminusA.z == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.y) - ((PminusA.z) * (BminusA.y) / (BminusA.z));
		float betafromgamma = ((CminusA.z) * (BminusA.y) / (BminusA.z));
		float multiplier = -betafromgamma + (CminusA.y);
		gamma = constantterm / multiplier;
		beta = (PminusA.z - gamma * (CminusA.z)) / (BminusA.z);
	}
	else{
		if(BminusA.x == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.y) - ((PminusA.x) * (BminusA.y) / (BminusA.x));
		float betafromgamma = ((CminusA.x) * (BminusA.y) / (BminusA.x));
		float multiplier = -betafromgamma + (CminusA.y);
		gamma = constantterm / multiplier;
		beta = (PminusA.x - gamma * (CminusA.x)) / (BminusA.x);
	}
	if ((beta + gamma <= 1) && (beta >= 0) && (gamma >= 0)) {
		/*
		std::clog<<"passed\n";
		std::clog<<"Stats: "<<beta + gamma<<", "<<beta<<", "<<gamma<<"\n";
		std::clog<<"Inters: ";
		printvec3(inters);
		std::clog<<"\n";
		*/
		point_int->x = inters.x;
		point_int->y = inters.y;
		point_int->z = inters.z;
		closest = t;
		normal.x = this->normal.x;
		normal.y = this->normal.y;
		normal.z = this->normal.z;
		return true;
	}
	return false;
}

bool Sphere::blockCheck(float closest, myvec3 raydir, myvec3 position) {
	myvec3 p1 = this->inv_transform * raydir;
	myvec4 p0 = this->inv_transform * myvec4(position.x, position.y, position.z, 1);
	myvec3 P0minusC = myvec3(p0.x, p0.y, p0.z) - this->position;
	float a = dot(p1, p1);
	float b = 2 * dot(p1, P0minusC);
	float c = dot(P0minusC, P0minusC) - this->radius * this->radius;
	float checker = b * b - 4 * a * c;
	if (checker >= 0) {
		float sqrtres = sqrt(checker);
		float distsmall = (-b - sqrtres) / (2 * a);
		if (distsmall > 0 && distsmall < closest) return false;
		else{
			float distbig = (-b + sqrtres) / (2 * a);
			if (distbig > 0 && distbig < closest){
				return false;
			}
		}
	}
	return true;
}

bool Triangle::blockCheck(float closest, myvec3 raydir, myvec3 position) {
	float P1DotN = dot(raydir, this->normal);
	if (P1DotN == 0) return true;
	float t = (dot(this->vertA, this->normal) - dot(position, this->normal)) / P1DotN;
	if(t>=closest || t < 0) return true;
	myvec3 inters = (t * raydir) + position;
	/*
	std::clog<<"Inters: ";
	printvec3(inters);
	std::clog<<"\n";
	*/
	myvec3 PminusA = inters - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	myvec3 BminusA = myvec3(this->vertB.x, this->vertB.y, this->vertB.z) - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	myvec3 CminusA = myvec3(this->vertC.x, this->vertC.y, this->vertC.z) - myvec3(this->vertA.x, this->vertA.y, this->vertA.z);
	float beta = -1;
	float gamma = -1;
	if(BminusA.y == 0 && CminusA.y == 0){
		if(BminusA.x == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.z) - ((PminusA.x) * (BminusA.z) / (BminusA.x));
		float betafromgamma = ((CminusA.x) * (BminusA.z) / (BminusA.x));
		float multiplier = -betafromgamma + (CminusA.z);
		gamma = constantterm / multiplier;
		beta = (PminusA.x - gamma * (CminusA.x)) / (BminusA.x);
	}
	else if(BminusA.x == 0 && CminusA.x == 0){
		if(BminusA.z == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.y) - ((PminusA.z) * (BminusA.y) / (BminusA.z));
		float betafromgamma = ((CminusA.z) * (BminusA.y) / (BminusA.z));
		float multiplier = -betafromgamma + (CminusA.y);
		gamma = constantterm / multiplier;
		beta = (PminusA.z - gamma * (CminusA.z)) / (BminusA.z);
	}
	else{
		if(BminusA.x == 0){
			myvec3 temp = BminusA;
			BminusA = CminusA;
			CminusA = temp;
		}
		float constantterm = (PminusA.y) - ((PminusA.x) * (BminusA.y) / (BminusA.x));
		float betafromgamma = ((CminusA.x) * (BminusA.y) / (BminusA.x));
		float multiplier = -betafromgamma + (CminusA.y);
		gamma = constantterm / multiplier;
		beta = (PminusA.x - gamma * (CminusA.x)) / (BminusA.x);
	}
	if ((beta + gamma <= 1) && (beta >= 0) && (gamma >= 0)) {
		return false;
	}
	return true;
}

float helper_pos(mymat4 m4, int i){
	float back = m4.content[i][3];
	float bottom = m4.content[3][3];
	float front = m4.content[i][i];
	return (back + sqrtf(back * back - (bottom * front))) / bottom;
}

float helper_neg(mymat4 m4, int i){
	float back = m4.content[i][3];
	float bottom = m4.content[3][3];
	float front = m4.content[i][i];
	return (back - sqrtf(back * back - (bottom * front))) / bottom;
}

void Sphere::Bound(){
	mymat4 s_mat_inv = mymat4(1/this->radius);
	s_mat_inv.content[3][3] = -1.f;

	mymat4 R = this->transform * s_mat_inv * transpose(this->transform);
	this->topright.x = helper_neg(R, 0);
	this->topright.y = helper_neg(R, 1);
	this->topright.z = helper_neg(R, 2);

	this->bottomleft.x = helper_pos(R, 0);
	this->bottomleft.y = helper_pos(R, 1);
	this->bottomleft.z = helper_pos(R, 2);
}

void Triangle::Bound(){
	this->bottomleft.x = std::min(this->vertA.x, this->vertB.x);
	this->bottomleft.x = std::min(this->bottomleft.x, this->vertC.x);
	this->bottomleft.y = std::min(this->vertA.y, this->vertB.y);
	this->bottomleft.y = std::min(this->bottomleft.y, this->vertC.y);
	this->bottomleft.z = std::min(this->vertA.z, this->vertB.z);
	this->bottomleft.z = std::min(this->bottomleft.z, this->vertC.z);

	this->topright.x = std::max(this->vertA.x, this->vertB.x);
	this->topright.x = std::max(this->topright.x, this->vertC.x);
	this->topright.y = std::max(this->vertA.y, this->vertB.y);
	this->topright.y = std::max(this->topright.y, this->vertC.y);
	this->topright.z = std::max(this->vertA.z, this->vertB.z);
	this->topright.z = std::max(this->topright.z, this->vertC.z);
	// std::cout<<"Triangle vertices: ";
	// printvec4(vertA);
	// std::cout<<", ";
	// printvec4(vertB);
	// std::cout<<", ";
	// printvec4(vertC);
	// std::cout<<"\nTriangle bound: ";
	// printvec3(bottomleft);
	// std::cout<<", ";
	// printvec3(topright);
	// std::cout<<"\n\n";
}