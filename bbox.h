#pragma once
#include "matvec.h"
#include "libmorton/morton.h"
#include "objects.h"
#include <iostream>

//bounding box
//We'll implement the top-down construction

class BBox{
	public:
		bool isleaf;
		int max_obj_amt;
		Object* obj;
		BBox* child_A;
		BBox* child_B;

		//Splits this current box into 2 nodes
		virtual void split(){
			return;
		}

		BBox(){
			obj = nullptr;
			child_A = nullptr;
			child_B = nullptr;
		};

		BBox(int max_amt){
			BBox();
			max_obj_amt = max_amt;
		};
		
		// virtual void traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
		// 	return;
		// }

		virtual void printcheck(){
			std::cout<<"Default box abstract\n";
		};

		
		virtual void unionBounds(Object* obj){
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
		
		void unionBounds(Object* obj){
			this->bottomleft.x = std::min(this->bottomleft.x, obj->bottomleft.x);
			this->bottomleft.y = std::min(this->bottomleft.y, obj->bottomleft.y);
			this->bottomleft.z = std::min(this->bottomleft.z, obj->bottomleft.z);
		
			this->topright.x = std::max(this->topright.x, obj->topright.x);
			this->topright.y = std::max(this->topright.y, obj->topright.y);
			this->topright.z = std::max(this->topright.z, obj->topright.z);
		}

		void printcheck(){
			std::cout<<"\nbottomleft\n";
			printvec3(bottomleft);
			std::cout<<"\ntopright\n";
			printvec3(topright);
			std::cout<<"\n";
		}
};

