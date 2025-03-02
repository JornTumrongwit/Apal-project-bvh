#pragma once
#include "matvec.h"
#include "libmorton/morton.h"
#include "objects.h"
#include <iostream>

//bounding box
class BBox{
	public:
		Object* obj;
		BBox() = default;
		BBox(BBox* bb1, BBox* bb2){ BBox(); };

		BBox(myvec3 bottomleft, myvec3 topright, Object* this_obj){
			obj = this_obj;
		};

		// virtual void traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position){
		// 	return;
		// }

		virtual void printcheck(){
			std::cout<<"Default box abstract\n";
		};
};

//Simple box using vec3 as the coordinates
class SimpleBBox: public BBox{
	public:
		myvec3 bottomleft;
		myvec3 topright;

		//void traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position);

		SimpleBBox(SimpleBBox* bb1, SimpleBBox* bb2){
			this->bottomleft = myvec3(std::min(bb1->bottomleft.x, bb2->bottomleft.x), 
									std::min(bb1->bottomleft.y, bb2->bottomleft.y),
									std::min(bb1->bottomleft.z, bb2->bottomleft.z));
			this->topright = myvec3(std::max(bb1->topright.x, bb2->topright.x), 
								std::max(bb1->topright.y, bb2->topright.y),
								std::max(bb1->bottomleft.z, bb2->topright.z));
		};

		SimpleBBox(Object* this_obj){
			this->bottomleft = myvec3(0, 0, 0);
			this->topright = myvec3(0, 0, 0);
			this_obj->Bound(&this->bottomleft, &this->topright);
			this->obj = this_obj;
		};

		void printcheck(){
			std::cout<<"\nbottomleft\n";
			printvec3(bottomleft);
			std::cout<<"\ntopright\n";
			printvec3(topright);
			std::cout<<"\n";
		}
};

