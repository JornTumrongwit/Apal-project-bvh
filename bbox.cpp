#include "bbox.h"
#include "variables.h"
#include <iostream>

//Bounding Box linear compaction
void BBox::compact(){
    int index = compactBBoxes.size();
    CBB compacted = this->compactor();
    compactBBoxes.push_back(compacted);
    if(amt < 0){
        //compact child 1
        this->child[0]->compact();

        //save index to child 2
        compactBBoxes[index].child2 = compactBBoxes.size();

        //compact child 2
        this->child[1]->compact();
    }
}

//giving a compact representation of simple bounding box
CBB SimpleBBox::compactor(){
    //Compact Bounding Box
// struct CBB{
// 	myvec3 bottomleft;
// 	myvec3 topright;
// 	int offset = -1;
// 	int amt = -1;
// 	//child 1 context is not need if it's just next to it
// 	int child2;
// };
    CBB cbb;
    cbb.bottomleft = this->bottomleft.copy();
    cbb.topright = this->topright.copy();
    cbb.offset = this->offset;
    cbb.amt = this->amt;
    cbb.child2 = -1;
    cbb.axis = this->axis;
    return cbb;
}

bool bboxTraverseRecursive(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, myvec3 inv_dir, int& obj, float& t_far, int& traverseCount, int offset){
    //First, check if the ray intersects this box
    //slab method
    CBB box = compactBBoxes[offset];
    myvec3 t_low =  (box.bottomleft - *position) * inv_dir;
    myvec3 t_high =  (box.topright - *position) * inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_close = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float this_t_far = std::min(t_far, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection or already found smth closer
    if(this_t_far < t_close) return false;

    bool intersected = false;
    //if this is a leaf, check the prims
    if(box.amt > 0){
        //std::cout<<"Checking leaf\n";
        traverseCount++;
        for(int i = box.offset; i < box.offset+box.amt; i++){
            Triangle tri = TriangleList[i];
            //std::cout<<"Check triangle " << i << " out of " << TriangleList.size()<< "\n";
            if(tri.CheckIntersect(closest, raydir, normal, point_int, position)){
                //std::cout<<"Intersection found\n";
                intersected = true;
                obj = i;
                t_far = closest;
            }
        }
        //if it intersects smth, return true
        //std::cout<<"returning leaf traversal as " << intersected << "\n";
        return intersected;
    }

    //proceed with traverse
    //We check which traversal to pick first by checking the splitting axis positive/negative values
    if ((*raydir)[box.axis] >= 0){
        intersected = bboxTraverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount, offset+1) || intersected;
        intersected = bboxTraverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount, box.child2) || intersected;
    }
    else{
        intersected = bboxTraverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount, box.child2) || intersected;
        intersected = bboxTraverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount, offset+1) || intersected;
    }
    //std::cout<<"returning node traversal as " << intersected << "\n";
    return intersected;
}

//traversing the compact bbox
bool bboxTraverser(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, int& obj, int& traverseCount, int offset){
    //First, check if the ray intersects this box
    //slab method
    myvec3 inv_dir = myvec3(1 / raydir->x, 1 / raydir->y, 1 / raydir->z);
    myvec3 t_low =  (compactBBoxes[offset].bottomleft - *position)*inv_dir;
    myvec3 t_high =  (compactBBoxes[offset].topright - *position)*inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_near = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float t_far = std::min(t_max.x, std::min(t_max.y, t_max.z));

    // if t_far < t_close, no intersection
    if(t_far < t_near) return false;
    //std::cout<<"Begin Traversal\n";
    //If there is an intersection, continue with the recursive version
    return bboxTraverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount, offset);
}

//visibility checking
bool cbbBlockRecursive(float closest, myvec3 raydir, myvec3 position, myvec3 inv_dir, int offset){
    CBB cbb = compactBBoxes[offset];
    //First, check if the ray intersects this box
    //slab method
    myvec3 t_low =  (cbb.bottomleft - position) * inv_dir;
    myvec3 t_high =  (cbb.topright - position) * inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_close = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float this_t_far = std::min(closest, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection or light source is closer
    if(this_t_far < t_close) return true;

    //if this is a leaf, check the prims
    if(cbb.amt > 0){
        for(Triangle tri: TriangleList){
            //Triangle tri = TriangleList[i];
            if(!tri.blockCheck(closest, raydir, position)) return false;
        }
        return true;
    }

    //proceed with traverse
    //We check which traversal to pick first by checking the splitting axis positive/negative values
    if ((raydir)[cbb.axis] >= 0){
        if(cbbBlockRecursive(closest, raydir, position, inv_dir, offset+1)){
            return cbbBlockRecursive(closest, raydir, position, inv_dir, cbb.child2);
        }
        return false;
    }
    else{
        if(cbbBlockRecursive(closest, raydir, position, inv_dir, cbb.child2)){
            return cbbBlockRecursive(closest, raydir, position, inv_dir, offset+1);
        }
        return false;
    }
}

bool cbbBlock(float closest, myvec3 raydir, myvec3 position, int offset){
    //First, check if the ray intersects this box
    //slab method
    myvec3 inv_dir = myvec3(1 / raydir.x, 1 / raydir.y, 1 / raydir.z);
    myvec3 t_low =  (compactBBoxes[0].bottomleft - position)*inv_dir;
    myvec3 t_high =  (compactBBoxes[0].topright - position)*inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_near = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float t_far = std::min(closest, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection
    if(t_far < t_near) return false;
    //std::cout<<"Begin Traversal\n";
    //If there is an intersection, continue with the recursive version
    return cbbBlockRecursive(closest, raydir, position, inv_dir, offset);
}

//Simple traversal
bool SimpleBBox::traverseRecursive(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, myvec3 inv_dir, int& obj, float& t_far, int& traverseCount){
    //First, check if the ray intersects this box
    //slab method
    myvec3 t_low =  (this->bottomleft - *position) * inv_dir;
    myvec3 t_high =  (this->topright - *position) * inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_close = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float this_t_far = std::min(t_far, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection or already found smth closer
    if(this_t_far < t_close) return false;

    bool intersected = false;
    //if this is a leaf, check the prims
    if(amt > 0){
        //std::cout<<"Checking leaf\n";
        traverseCount++;
        for(int i = offset; i < offset+amt; i++){
            Triangle tri = TriangleList[i];
            //std::cout<<"Check triangle " << i << " out of " << TriangleList.size()<< "\n";
            if(tri.CheckIntersect(closest, raydir, normal, point_int, position)){
                //std::cout<<"Intersection found\n";
                intersected = true;
                obj = i;
                t_far = closest;
            }
        }
        //if it intersects smth, return true
        //std::cout<<"returning leaf traversal as " << intersected << "\n";
        return intersected;
    }

    //proceed with traverse
    //We check which traversal to pick first by checking the splitting axis positive/negative values
    if ((*raydir)[this->axis] >= 0){
        intersected = child[0]->traverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount) || intersected;
        intersected = child[1]->traverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount) || intersected;
    }
    else{
        intersected = child[1]->traverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount) || intersected;
        intersected = child[0]->traverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount) || intersected;
    }
    //std::cout<<"returning node traversal as " << intersected << "\n";
    return intersected;
}

bool SimpleBBox::traverse(float& closest, myvec3* raydir, myvec3& normal, myvec3*& point_int, myvec3* position, int& obj, int& traverseCount){
    //First, check if the ray intersects this box
    //slab method
    myvec3 inv_dir = myvec3(1 / raydir->x, 1 / raydir->y, 1 / raydir->z);
    myvec3 t_low =  (this->bottomleft - *position)*inv_dir;
    myvec3 t_high =  (this->topright - *position)*inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_near = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float t_far = std::min(t_max.x, std::min(t_max.y, t_max.z));

    // if t_far < t_close, no intersection
    if(t_far < t_near) return false;
    //std::cout<<"Begin Traversal\n";
    //If there is an intersection, continue with the recursive version
    return this->traverseRecursive(closest, raydir, normal, point_int, position, inv_dir, obj, t_far, traverseCount);
}

bool SimpleBBox::blockingTraverseRecursive(float closest, myvec3 raydir, myvec3 position, myvec3 inv_dir){
    //First, check if the ray intersects this box
    //slab method
    myvec3 t_low =  (this->bottomleft - position) * inv_dir;
    myvec3 t_high =  (this->topright - position) * inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_close = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float this_t_far = std::min(closest, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection or already found smth closer
    if(this_t_far <= t_close) return true;

    //if this is a leaf, check the prims
    if(amt > 0){
        for(Triangle tri: TriangleList){
            //Triangle tri = TriangleList[i];
            if(!tri.blockCheck(closest, raydir, position)) return false;
        }
        return true;
    }

    //proceed with traverse
    //We check which traversal to pick first by checking the splitting axis positive/negative values
    if ((raydir)[this->axis] >= 0){
        if(child[0]->blockingTraverseRecursive(closest, raydir, position, inv_dir)){
            return child[1]->blockingTraverseRecursive(closest, raydir, position, inv_dir);
        }
        else return false;
    }
    else{
        if(child[1]->blockingTraverseRecursive(closest, raydir, position, inv_dir)){
            return child[0]->blockingTraverseRecursive(closest, raydir, position, inv_dir);
        }
        else return false;
    }
    return true;
}

bool SimpleBBox::blockingTraverse(float closest, myvec3 raydir, myvec3 position){
    //First, check if the ray intersects this box
    //slab method
    myvec3 inv_dir = myvec3(1 / raydir.x, 1 / raydir.y, 1 / raydir.z);
    myvec3 t_low =  (this->bottomleft - position)*inv_dir;
    myvec3 t_high =  (this->topright - position)*inv_dir;

    myvec3 t_min = myvec3(t_low.x, t_low.y, t_low.z);
    myvec3 t_max = myvec3(t_high.x, t_high.y, t_high.z);

    t_min.x = std::min(t_low.x, t_high.x);
    t_min.y = std::min(t_low.y, t_high.y);
    t_min.z = std::min(t_low.z, t_high.z);
    t_max.x = std::max(t_low.x, t_high.x);
    t_max.y = std::max(t_low.y, t_high.y);
    t_max.z = std::max(t_low.z, t_high.z);

    float t_near = std::max(t_min.x, std::max(t_min.y, t_min.z));
    float t_far = std::min(closest, std::min(t_max.x, std::min(t_max.y, t_max.z)));

    // if t_far < t_close, no intersection
    if(t_far < t_near) return false;
    //std::cout<<"Begin Traversal\n";
    //If there is an intersection, continue with the recursive version
    return this->blockingTraverseRecursive(closest, raydir, position, inv_dir);
}

//Naive splitting function
void SimpleBBox::splitRecursion(std::span<Triangle> tri_span, int offset){
    //Find which axis
    myvec3 axes = centright - centleft;
    this->axis = 0;
    if (axes.y > axes.x){
        if (axes.z > axes.y) {
            this->axis = 2;
        }
        else this->axis = 1;
    }
    else if (axes.z > axes.x){
        this->axis = 2;
    }

    //if nothing here, why bother
    if (tri_span.size() <= 0) return;

    //if leaf is small enough, return
    if (tri_span.size() <= max_obj_amt){
        // for(Triangle &tri: tri_span){
        //     tri.Bound();
        // }
        this->offset = offset;
        this->amt = tri_span.size();
        this->bounding(tri_span);
        // std::cout<<"Got leaf of "<<this->amt << " items at offset " << this->offset<<"\n";
        // std::cout<<"With bounds ";
        // printvec3(this->bottomleft);
        // std::cout<<", ";
        // printvec3(this->topright);
        // std::cout<<"\n\n";
        return;
    }
    //find midpoint
    float midpoint = (centright[this->axis] + centleft[this->axis]) / 2;

    //case to run
    //Run mid split first, then centroid balancing
    int splitmethod = 1;
    int mid;
    int i = this->axis;
    switch(splitmethod){
        case 1:{
            //Partition based on center of axis
            auto midIter = std::partition(tri_span.begin(), tri_span.end(),
            [i, midpoint](Triangle &tri) {
                return tri.centroid()[i] < midpoint;
            });

            //Get the midpoint
            mid = midIter - tri_span.begin();
            //if this is a good split, break case. If not, fall down
            if (midIter != tri_span.begin() && midIter != tri_span.end())
                break;
        }
        case 0:{
            //partition based on centroid density
            mid = tri_span.size()/2;

            std::nth_element(tri_span.begin(), tri_span.begin() + mid, 
            tri_span.end(),
                  [i](Triangle &a, Triangle &b) { 
                      return a.centroid()[i] < b.centroid()[i];
                  });
            
            break;
        } 
    }

    //std::cout<<"Splitting "<<offset<<" of size " << tri_span.size() << " with mid " << mid << "\n";

    child[0] = new SimpleBBox(this->max_obj_amt);
    for(int ax = 0; ax < 3; ax++){
        child[0]->centleft[ax] = this->centleft[ax];
    }
    for(int ax = 0; ax < 3; ax++){
        child[0]->centright[ax] = this->centright[ax];
    }
    child[0]->centright[i] = midpoint;
    child[0]->splitRecursion(tri_span.subspan(0, mid), offset);

    child[1] = new SimpleBBox(this->max_obj_amt);
    for(int ax = 0; ax < 3; ax++){
        child[1]->centleft[ax] = this->centleft[ax];
    }
    for(int ax = 0; ax < 3; ax++){
        child[1]->centright[ax] = this->centright[ax];
    }
    child[1]->centleft[i] = midpoint;
    child[1]->splitRecursion(tri_span.subspan(mid), offset+mid);

    //children are done, rebound
    this->bounding(tri_span);
    // std::cout<<"From node of size "<<tri_span.size()<<"\n";
    // std::cout<<"That contains ";
    // printvec3(this->bottomleft);
    // std::cout<<", ";
    // printvec3(this->topright);
    // std::cout<<"\nChild at ";
    // printvec3(child[0]->centleft);
    // std::cout<<", ";
    // printvec3(child[0]->centright);
    // std::cout<<"\nwith offset ";
    // std::cout<<child[0]->offset<<" of size "<<child[0]->amt<<"\nand ";
    // printvec3(child[1]->centleft);
    // std::cout<<", ";
    // printvec3(child[1]->centright);
    // std::cout<<"\nwith offset ";
    // std::cout<<child[1]->offset<<" of size "<<child[1]->amt<<"\n\n";
}

void SimpleBBox::split(){
    std::span<Triangle> tri_span(TriangleList); 

    //bound the centroids
    this->centroidBounding(tri_span);

    //call the recursive function
    this->splitRecursion(tri_span, 0);
}

void SimpleBBox::centroidBounding(std::span<Triangle> triangles){
    for(Triangle &tri: triangles){
        myvec3 cent = tri.centroid();
        this->centleft.x = std::min(this->centleft.x, cent.x);
        this->centleft.y = std::min(this->centleft.y, cent.y);
        this->centleft.z = std::min(this->centleft.z, cent.z);
    
        this->centright.x = std::max(this->centright.x, cent.x);
        this->centright.y = std::max(this->centright.y, cent.y);
        this->centright.z = std::max(this->centright.z, cent.z);
    }
}

void SimpleBBox::bounding(std::span<Triangle> tri_span){
    //triangles should be here now
    for(Triangle &tri: tri_span){
        unionBounds(tri);
    }
}

void MortonBBox::split(){
    //find the min/max
    myvec3 min = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    myvec3 max = myvec3(FLT_MIN, FLT_MIN, FLT_MIN);
    for(Triangle tri: TriangleList){
        tri.Bound();
        myvec3 centroid = tri.centroid();
        min.x = std::min(min.x, centroid.x);
        min.y = std::min(min.y, centroid.y);
        min.z = std::min(min.z, centroid.z);

        max.x = std::max(max.x, centroid.x);
        max.y = std::max(max.y, centroid.y);
        max.z = std::max(max.z, centroid.z);
    }

    int maxval = 2047;
    //store
    std::vector<mortonObj> mObj;
    for(int i = 0; i<TriangleList.size(); i++){
        Triangle tri = TriangleList[i];
        myvec3 centroid = tri.centroid();
        myvec3 normalized = myvec3((((centroid.x - min.x) / (max.x-min.x))* maxval), 
                                    (((centroid.y - min.y) / (max.y-min.y))* maxval),
                                    (((centroid.z - min.z) / (max.z-min.z))* maxval));
        mortonObj mo;
        mo.morton = libmorton::morton3D_32_encode((int) normalized.x, (int) normalized.y, (int) normalized.z);
        mo.offset = i;
        mObj.push_back(mo);
    }
    //make a vector of all triangles
    std::vector<mortonObj> mObj;
    for(Triangle tri: TriangleList){

    }
}
