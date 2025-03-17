#pragma once
#include "matvec.h"
#include "libmorton/morton.h"
#include "objects.h"
#include <iostream>
#include <span> 
#include <vector>
#define FLT_MIN          1.175494351e-38F  
#define FLT_MAX          3.402823466e+38F 
enum AXIS { X, Y, Z };

//Objects Morton'd
struct mortonObj{
	uint_fast32_t morton;
	//offset to the actual thing
	int offset;
};

//bounding top right and bottom left
struct Bounds{
	myvec3 bottomleft;
	myvec3 topright;
	float SurfaceArea();
};

//Compact Bounding Box
struct CBB{
	Bounds b;
	int offset = -1;
	int amt = -1;
	//child 1 context is not need if it's just next to it
	int child2;
	int axis;
};

//stripped down version of BBox for building
class BBuild{
	public:
		void leaf(int first, int n, myvec3 bl, myvec3 tr){
			this->b.topright = tr;
			this->b.bottomleft = bl;
			this->amt = n;
			this->offset = first;
		};

		void interior(int a, BBuild* c1, BBuild* c2);

		void unionBounds(Triangle obj){
			this->b.bottomleft.x = std::min(this->b.bottomleft.x, obj.bottomleft.x);
			this->b.bottomleft.y = std::min(this->b.bottomleft.y, obj.bottomleft.y);
			this->b.bottomleft.z = std::min(this->b.bottomleft.z, obj.bottomleft.z);
		
			this->b.topright.x = std::max(this->b.topright.x, obj.topright.x);
			this->b.topright.y = std::max(this->b.topright.y, obj.topright.y);
			this->b.topright.z = std::max(this->b.topright.z, obj.topright.z);
		}

		void bounding(std::span<Triangle> t_span);
		Bounds b;

		BBuild* child[2];
		int axis, offset, amt;

		CBB compactor();

		void compact();
};

//struct helper for morton implementation
struct LBVHTreelet{
	size_t start, prim_amt;
	BBuild* nodes;
};

//bounding box
//We'll implement the top-down construction
//Let's only work with triangles
class BBox{
	public:
		int max_obj_amt = 5;
		int offset = -1;
		int amt = -1;
		BBox* child[2];
		int axis;

		Bounds b;

		BBox() = default;

		BBox(int max_amt){
			BBox();
			max_obj_amt = max_amt;
		};

		//Splits this current box into 2 nodes
		virtual void split(){
			return;
		}
		

		virtual void splitRecursion(std::span<Triangle> tri_span, int offset){
			return;
		}

		virtual void printcheck(){
			std::cout<<"Default box abstract\n";
		};

		
		virtual void unionBounds(Triangle obj){
			return;
		}

		virtual CBB compactor(){
			CBB cbb;
			return cbb;
		};

		virtual void compact(){
			return;
		};
};

//Simple box using vec3 as the coordinates
class SimpleBBox: public BBox{
	public:
		SimpleBBox(){
			b.bottomleft = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
			b.topright = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		}

		SimpleBBox(int max_amt){
			SimpleBBox();
			this->max_obj_amt = max_amt;
		}
		
		void unionBounds(Triangle obj){
			this->b.bottomleft.x = std::min(this->b.bottomleft.x, obj.bottomleft.x);
			this->b.bottomleft.y = std::min(this->b.bottomleft.y, obj.bottomleft.y);
			this->b.bottomleft.z = std::min(this->b.bottomleft.z, obj.bottomleft.z);
		
			this->b.topright.x = std::max(this->b.topright.x, obj.topright.x);
			this->b.topright.y = std::max(this->b.topright.y, obj.topright.y);
			this->b.topright.z = std::max(this->b.topright.z, obj.topright.z);
		}

		void printcheck(){
			std::cout<<"\nbottomleft\n";
			printvec3(b.bottomleft);
			std::cout<<"\ntopright\n";
			printvec3(b.topright);
			std::cout<<"\n";
		}

		void split();

		void centroidBounding(std::span<Triangle> triangles);

		void bounding(std::span<Triangle> tri_span);

		void splitRecursion(std::span<Triangle> tri_span, int offset);

		void compact();

		CBB compactor();
};

//Morton-based box
class MortonBBox: public BBox{
	public:
		MortonBBox(){
			b.bottomleft = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
			b.topright = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		}

		MortonBBox(int max_amt){
			MortonBBox();
			this->max_obj_amt = max_amt;
		}
		
		void unionBounds(Triangle obj){
			this->b.bottomleft.x = std::min(this->b.bottomleft.x, obj.bottomleft.x);
			this->b.bottomleft.y = std::min(this->b.bottomleft.y, obj.bottomleft.y);
			this->b.bottomleft.z = std::min(this->b.bottomleft.z, obj.bottomleft.z);
		
			this->b.topright.x = std::max(this->b.topright.x, obj.topright.x);
			this->b.topright.y = std::max(this->b.topright.y, obj.topright.y);
			this->b.topright.z = std::max(this->b.topright.z, obj.topright.z);
		}

		void printcheck(){
			std::cout<<"\nbottomleft\n";
			printvec3(b.bottomleft);
			std::cout<<"\ntopright\n";
			printvec3(b.topright);
			std::cout<<"\n";
		}

		void split();

		void centroidBounding(std::span<Triangle> triangles){
			return;
		}

		void bounding(std::span<Triangle> tri_span){
			return;
		}

		void compact();
};

bool bboxTraverser(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, int& obj, int& traverseCount, int offset);
bool bboxTraverseRecursive(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, myvec3 inv_dir, int& obj, float& t_far, int& traverseCount, int offset);
bool cbbBlock(float closest, myvec3 raydir, myvec3 position, int offset);
bool cbbBlockRecursive(float closest, myvec3 raydir, myvec3 position, myvec3 inv_dir, int offset);
Bounds Union(Bounds b1, Bounds b2);