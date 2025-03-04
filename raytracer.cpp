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

const float epsilon = 0.001;

myvec3 raytracer(myvec3 raydir, myvec3 position, float depth)
{
	if(depth > reflectdepth){
		return myvec3(0, 0, 0);
	}
	Sphere hitsph;
	Triangle hittri;
	Object* hitObj;
	myvec3 tempnormal = myvec3(0, 0, 0);
	myvec3 point_int = myvec3(0, 0, 0);
	myvec3* inter = &point_int;
	myvec3 pixels = myvec3(0, 0, 0);
	float closest = std::numeric_limits<float>::max();
	SHAPE intersect = NONE;
	for (Object* testObj : ObjectList) {
		if(testObj->CheckIntersect(closest, &raydir, tempnormal, inter, &position)){
			intersect = testObj -> poly_type;
			hitObj = testObj;
		}
	}
	// for (Sphere testSphere : SphereList) {
	// 	if(testSphere.CheckIntersect(closest, &raydir, tempnormal, inter, &position)){
	// 		intersect = SPHERE;
	// 		hitObj = &testSphere;
	// 	}
	// }
	// for (Triangle testTriangle : TriangleList) {
	// 	if(testTriangle.CheckIntersect(closest, &raydir, tempnormal, inter, &position)){
	// 		intersect = TRIANGLE;
	// 		hitObj = &testTriangle;
	// 	}
	// }
	if (intersect == TRIANGLE) {
		get_color_tri(raydir, point_int, static_cast<Triangle*>(hitObj), position, tempnormal, pixels, depth);
	}
	else if (intersect == SPHERE) {
		get_color_sph(raydir, point_int, static_cast<Sphere*>(hitObj), position, tempnormal, pixels, depth);
	}
	return pixels;
}

void get_color_tri(myvec3 raydir, myvec3 intersect, Triangle* hittri, myvec3 position, myvec3 viewnormal, myvec3& pixels, float depth) {
	myvec3 eyedirn = normalize(position - intersect);

	// Compute normal
	myvec3 normal = normalize(viewnormal);

	get_color(raydir, position, intersect, eyedirn, normal, toVec3(hittri->diffuse), toVec3(hittri->specular), hittri->shininess, hittri->ambient, hittri->emission, pixels, depth);
}

void get_color_sph(myvec3 raydir, myvec3 intersect, Sphere* hitsph, myvec3 position, myvec3 getnormal, myvec3& pixels, float depth) {
	myvec3 viewnormal = transpose(hitsph->inv_transform) * getnormal;
	myvec3 eyedirn = normalize(position - intersect);

	// Compute normal
	myvec3 normal = normalize(viewnormal);

	get_color(raydir, position, intersect, eyedirn, normal, toVec3(hitsph->diffuse), toVec3(hitsph->specular), hitsph->shininess, hitsph->ambient, hitsph->emission, pixels, depth);
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
	for (Object* testObj : ObjectList){
		if(!testObj->blockCheck(closest, raydir, position)) return false;
	}
	// for (Sphere testSphere : SphereList) {
	// 	if(!testSphere.blockCheck(closest, raydir, position)) return false;
	// }
	
	// for (Triangle testTriangle : TriangleList) {
	// 	if (!testTriangle.blockCheck(closest, raydir, position)) return false;
	// }
	return true;
}

myvec3 toVec3(float* inp) {
	return myvec3(inp[0], inp[1], inp[2]);
}