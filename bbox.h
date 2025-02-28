#include "matvec.h"
#include "libmorton/morton.h"
#include "objects.h"


//bounding box
class BBox{
	void makeBox(BBox left, BBox right){
		return;
	}

	void traverse(myvec3 raydir, myvec3 raypops){
		return;
	}

	Object obj;
};

class SimpleBBox: public BBox{

};

