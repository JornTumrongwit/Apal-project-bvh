#pragma once
#include "matvec.h"
#include "libmorton/morton.h"
#include "objects.h"
#include <iostream>
#include <span> 

enum AXIS { X, Y, Z };

//bounding box
//We'll implement the top-down construction
//Let's only work with triangles
class BBox{
	public:
		bool isleaf;
		int max_obj_amt = 5;
		int offset = -1;
		int amt;
		BBox* child[2];

		//Splits this current box into 2 nodes
		virtual void split(){
			return;
		}

		BBox() = default;

		BBox(int max_amt){
			BBox();
			max_obj_amt = max_amt;
		};
		
		// virtual void traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
		// 	return;
		// }

		virtual void splitRecursion(AXIS a, std::span<Triangle> tri_span, int offset){
			return;
		}

		virtual void printcheck(){
			std::cout<<"Default box abstract\n";
		};

		
		virtual void unionBounds(Triangle obj){
			return;
		}
};

//Simple box using vec3 as the coordinates
class SimpleBBox: public BBox{
	public:
		myvec3 bottomleft;
		myvec3 topright;

		//void traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position);

		SimpleBBox() = default;

		SimpleBBox(int max_amt){
			SimpleBBox();
			this->max_obj_amt = max_amt;
		}
		
		void unionBounds(Triangle obj){
			this->bottomleft.x = std::min(this->bottomleft.x, obj.bottomleft.x);
			this->bottomleft.y = std::min(this->bottomleft.y, obj.bottomleft.y);
			this->bottomleft.z = std::min(this->bottomleft.z, obj.bottomleft.z);
		
			this->topright.x = std::max(this->topright.x, obj.topright.x);
			this->topright.y = std::max(this->topright.y, obj.topright.y);
			this->topright.z = std::max(this->topright.z, obj.topright.z);
		}

		void printcheck(){
			std::cout<<"\nbottomleft\n";
			printvec3(bottomleft);
			std::cout<<"\ntopright\n";
			printvec3(topright);
			std::cout<<"\n";
		}

		void split();

		void splitRecursion(AXIS a, std::span<Triangle> tri_span, int offset);
};

