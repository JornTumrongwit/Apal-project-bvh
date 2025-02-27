#include "Transform.h"
#include "raytracer.h"
#include "variables.h"
#include <cmath>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

/*
class Object {       // The class
public:             // Access specifier
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float emission[3];
	float shininess;
	Object() = default;
};

class Sphere : public Object {
public:
	myvec3 position;
	float radius;
	mymat4 transform;
	mymat4 inv_transform;

	Sphere() = default;
	void CheckIntersect(Sphere& target, float& closest, myvec3 raydir, myvec4& point_int, SHAPE& intersect, myvec3 position);
};

class Triangle : public Object {
public:
	myvec4 vertA;
	myvec4 vertB;
	myvec4 vertC;
	myvec3 normal;

	Triangle() = default;
	void CheckIntersect(Triangle& target, float& closest, myvec3 raydir, myvec4& point_int, SHAPE& intersect, myvec3 position);
};
*/

const float epsilon = 0.001;

myvec3 raytracer(myvec3 raydir, myvec3 position, float depth)
{
	if(depth > reflectdepth){
		return myvec3(0, 0, 0);
	}
	Sphere hitsph;
	Triangle hittri;
	myvec3 tempnormal = myvec3(0, 0, 0);
	myvec3 point_int = myvec3(0, 0, 0);
	myvec3* inter = &point_int;
	myvec3 pixels = myvec3(0, 0, 0);
	float closest = std::numeric_limits<float>::max();
	SHAPE intersect = NONE;
	for (Sphere testSphere : SphereList) {
		CheckIntersect(hitsph, closest, &raydir, tempnormal, inter, intersect, &position, &testSphere);
	}
	for (Triangle testTriangle : TriangleList) {
		CheckIntersect(hittri, closest, &raydir, tempnormal, inter, intersect, &position, &testTriangle);
	}
	if (intersect == TRIANGLE) {
		get_color_tri(raydir, point_int, hittri, position, tempnormal, pixels, depth);
	}
	else if (intersect == SPHERE) {
		get_color_sph(raydir, point_int, hitsph, position, tempnormal, pixels, depth);
	}
	return pixels;
}

void CheckIntersect(Triangle& target, float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, SHAPE& intersect, myvec3* position, Triangle* input) {
	//std::clog << "called triangle\n";
	//std::clog << "raydir: " << raydir.x << " "<<raydir.y << " "<<raydir.z << "\n";
	float P1DotN = dot(*raydir, input->normal);
	if (P1DotN == 0) return;
	float t = (dot(input->vertA, input->normal) - dot(*position, input->normal)) / P1DotN;
	if(t>=closest || t<0) return;
	myvec3 inters = (t * *raydir) + *position;
	/*
	std::clog<<"Inters: ";
	printvec3(inters);
	std::clog<<"\n";
	*/
	myvec3 PminusA = inters - myvec3(input->vertA.x, input->vertA.y, input->vertA.z);
	myvec3 BminusA = myvec3(input->vertB.x, input->vertB.y, input->vertB.z) - myvec3(input->vertA.x, input->vertA.y, input->vertA.z);
	myvec3 CminusA = myvec3(input->vertC.x, input->vertC.y, input->vertC.z) - myvec3(input->vertA.x, input->vertA.y, input->vertA.z);
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
		copytri(input, target);
		intersect = TRIANGLE;
		point_int->x = inters.x;
		point_int->y = inters.y;
		point_int->z = inters.z;
		closest = t;
		normal.x = input->normal.x;
		normal.y = input->normal.y;
		normal.z = input->normal.z;
	}
	else{
		//std::clog<<"failed\n";
	}
}

void copytri(Triangle* input, Triangle& out){
	for(int i = 0; i<3; i++){
		out.ambient[i] = input->ambient[i];
		out.diffuse[i] = input->diffuse[i];
		out.specular[i] = input->specular[i];
		out.emission[i] = input->emission[i];
		/*
		out->vertA[i] = input->vertA[i];
		out->vertB[i] = input->vertB[i];
		out->vertC[i] = input->vertC[i];
		out->normal[i] = input->normal[i];
		*/
	}
	out.shininess = input->shininess;
	out.vertA = input->vertA;
	out.vertB = input->vertB;
	out.vertC = input->vertC;
	out.normal.x = input->normal.x;
}

void CheckIntersect(Sphere& target, float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, SHAPE& intersect, myvec3* position, Sphere* input) {
	/*
	std::clog<<"raydir: ";
	printvec3(*raydir);
	std::clog<<"\n";
	*/
	myvec3 p1 = input->inv_transform * *raydir;
	/*
	std::clog<<"P1: ";
	printvec3(p1);
	std::clog<<"\n";*/
	myvec4 p0 = input->inv_transform * myvec4(position->x, position->y, position->z, 1);
	/*
	std::clog<<"treye: ";
	printvec4(treye);
	std::clog<<"\n";
	*/
	myvec3 P0minusC = myvec3(p0.x, p0.y, p0.z) - input->position;
	float a = dot(p1, p1);
	float b = 2 * dot(p1, P0minusC);
	float c = dot(P0minusC, P0minusC) - input->radius * input->radius;
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
				myvec4 inters = input->transform * oldinters;
				copysph(input, target);
				intersect = SPHERE;
				point_int->x = inters.x;
				point_int->y = inters.y;
				point_int->z = inters.z;
				closest = distsmall;
				myvec3 newnormal = myvec3(oldinters.x, oldinters.y, oldinters.z) - input->position;
				normal.x = newnormal.x;
				normal.y = newnormal.y;
				normal.z = newnormal.z;
			}
		}
		else {
			float distbig = (-b + sqrtres) / (2 * a);
			//std::clog<<distbig<<"\n";
			if (distbig > 0) {
				myvec4 oldinters = p0 + distbig * p1;
				myvec4 inters = input->transform * oldinters;
				if (distbig < closest) {
					copysph(input, target);
					intersect = SPHERE;
					point_int->x = inters.x;
					point_int->y = inters.y;
					point_int->z = inters.z;
					closest = distbig;
					myvec3 newnormal = myvec3(oldinters.x, oldinters.y, oldinters.z) - input->position;
					normal.x = newnormal.x;
					normal.y = newnormal.y;
					normal.z = newnormal.z;
				}
			}
		}
	}
}

void copysph(Sphere* input, Sphere& out){
	for(int i = 0; i<3; i++){
		out.ambient[i] = input->ambient[i];
		out.diffuse[i] = input->diffuse[i];
		out.specular[i] = input->specular[i];
		out.emission[i] = input->emission[i];
		/*
		out->vertA[i] = input->vertA[i];
		out->vertB[i] = input->vertB[i];
		out->vertC[i] = input->vertC[i];
		out->normal[i] = input->normal[i];
		*/
	}
	out.shininess = input->shininess;
	out.position = input->position;
	out.inv_transform = input->inv_transform;
	out.radius = input->radius;
	out.transform = input->transform;
}

void get_color_tri(myvec3 raydir, myvec3 intersect, Triangle hittri, myvec3 position, myvec3 viewnormal, myvec3& pixels, float depth) {
	myvec3 eyedirn = normalize(position - intersect);

	// Compute normal
	myvec3 normal = normalize(viewnormal);

	get_color(raydir, position, intersect, eyedirn, normal, toVec3(hittri.diffuse), toVec3(hittri.specular), hittri.shininess, hittri.ambient, hittri.emission, pixels, depth);
}

void get_color_sph(myvec3 raydir, myvec3 intersect, Sphere hitsph, myvec3 position, myvec3 getnormal, myvec3& pixels, float depth) {
	myvec3 viewnormal = transpose(hitsph.inv_transform) * getnormal;
	myvec3 eyedirn = normalize(position - intersect);

	// Compute normal
	myvec3 normal = normalize(viewnormal);

	get_color(raydir, position, intersect, eyedirn, normal, toVec3(hitsph.diffuse), toVec3(hitsph.specular), hitsph.shininess, hitsph.ambient, hitsph.emission, pixels, depth);
}

myvec3 ComputeLight(myvec3 direction, myvec3 lightcolor, myvec3 normal, myvec3 halfvec, myvec3 mydiffuse, myvec3 myspecular, float myshininess) {

	float nDotL = dot(normal, direction);
	myvec3 lambert = mydiffuse * lightcolor * std::max(nDotL, 0.0f);

	float nDotH = dot(normal, halfvec);
	myvec3 phong = myspecular * lightcolor * pow(std::max(nDotH, 0.0f), myshininess);

	myvec3 retval = lambert + phong;
	return retval;
}

void get_color(myvec3 raydir, myvec3 mypos, myvec3 intersect, myvec3 eyedirn, myvec3 normal, myvec3 diffuse, myvec3 specular, float shininess, float* ambience, float* emission, myvec3& endcolor, float depth) {
	//set base color to ambient and emission
	/*
	std::clog<<"Position: ";
	printvec3(intersect);
	std::clog<<"\n";
	*/
	myvec3 finalcolor = toVec3(ambience) + toVec3(emission);

	for (int i = 0; i < numused; i++) {
		myvec3 lightdirection;
		myvec3 position = myvec3(lightposn[i].x, lightposn[i].y, lightposn[i].z);
		float dist = distance(intersect, position);
		//std::clog<<dist<<"\n";
		float attdiv = 1;
		// set direction
		if (lightposn[i].w == 0) {
			dist = std::numeric_limits<float>::max();
			lightdirection = normalize(position);
		}
		else {
			lightdirection = normalize(position - intersect);
			attdiv = (1 / (attenuation[0] + dist * attenuation[1] + dist * dist * attenuation[2]));
		}
		if (blockCheck(dist, lightdirection, intersect + lightdirection*epsilon)){
			
			//std::clog<<"NOT BLOCKLED\n";
			//get halfway between normal and eye
			
			myvec3 half = normalize(lightdirection + eyedirn);

			
			//std::clog<<"ray: ";
			//printvec3(lightdirection);
			//std::clog<<"\n";
			//std::clog<<"position: ";
			//printvec3(position);
			//std::clog<<"\n";
			//std::clog<<"intersect: ";
			//printvec3(intersect);
			//std::clog<<"\n";

			myvec3 thiscol = ComputeLight(lightdirection, lightcolor[i], normal, half, diffuse, specular, shininess);
			
			//std::clog<<"COLOR: ";
			//printvec3(thiscol);
			//std::clog<<"\n";
			
			finalcolor = finalcolor + (thiscol * attdiv);
			//finalcolor = myvec3(0,1,0);
		}
		
		//else finalcolor = myvec3(1, 0, 0);
		
	}
	myvec3 recursiveray = reflect(raydir, normal);
	myvec3 fragColor = finalcolor + specular * raytracer(recursiveray, intersect + recursiveray*epsilon, depth+1); //TOTEST: reflection
	endcolor.x += fragColor.x;
	endcolor.y += fragColor.y;
	endcolor.z += fragColor.z;
}

//if return false, then there is something blocking it
bool blockCheck(float closest, myvec3 raydir, myvec3 position){
	
	for (Sphere testSphere : SphereList) {
		myvec3 p1 = testSphere.inv_transform * raydir;
		myvec4 p0 = testSphere.inv_transform * myvec4(position.x, position.y, position.z, 1);
		myvec3 P0minusC = myvec3(p0.x, p0.y, p0.z) - testSphere.position;
		float a = dot(p1, p1);
		float b = 2 * dot(p1, P0minusC);
		float c = dot(P0minusC, P0minusC) - testSphere.radius * testSphere.radius;
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
	}
	
	for (Triangle testTriangle : TriangleList) {
		float P1DotN = dot(raydir, testTriangle.normal);
		if (P1DotN == 0) continue;
		float t = (dot(testTriangle.vertA, testTriangle.normal) - dot(position, testTriangle.normal)) / P1DotN;
		if(t>=closest || t < 0) continue;
		myvec3 inters = (t * raydir) + position;
		/*
		std::clog<<"Inters: ";
		printvec3(inters);
		std::clog<<"\n";
		*/
		myvec3 PminusA = inters - myvec3(testTriangle.vertA.x, testTriangle.vertA.y, testTriangle.vertA.z);
		myvec3 BminusA = myvec3(testTriangle.vertB.x, testTriangle.vertB.y, testTriangle.vertB.z) - myvec3(testTriangle.vertA.x, testTriangle.vertA.y, testTriangle.vertA.z);
		myvec3 CminusA = myvec3(testTriangle.vertC.x, testTriangle.vertC.y, testTriangle.vertC.z) - myvec3(testTriangle.vertA.x, testTriangle.vertA.y, testTriangle.vertA.z);
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
	}
	return true;
}

myvec3 toVec3(float* inp) {
	return myvec3(inp[0], inp[1], inp[2]);
}