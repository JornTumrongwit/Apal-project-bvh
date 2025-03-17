#include "bbox.h"
#include "variables.h"
#include <iostream>
#include <bitset>
#include <atomic>

template <typename Predicate>
size_t FindInterval(size_t sz, const Predicate &pred) {
    using ssize_t = std::make_signed_t<size_t>;
    ssize_t size = (ssize_t)sz - 2, first = 1;
    while (size > 0) {
        size_t half = (size_t)size >> 1, middle = first + half;
        bool predResult = pred(middle);
        first = predResult ? middle + 1 : first;
        size = predResult ? size - (half + 1) : half;
    }
    return (size_t) std::clamp((int) first - 1, 0, (int) sz - 2);
}

Bounds Union(Bounds b1, Bounds b2){
    Bounds nb;
	nb.bottomleft.x = std::min(b1.bottomleft.x, b2.bottomleft.x);
	nb.bottomleft.y = std::min(b1.bottomleft.y, b2.bottomleft.y);
	nb.bottomleft.z = std::min(b1.bottomleft.z, b2.bottomleft.z);

	nb.topright.x = std::max(b1.topright.x, b2.topright.x);
	nb.topright.y = std::max(b1.topright.y, b2.topright.y);
	nb.topright.z = std::max(b1.topright.z, b2.topright.z);
	return nb;
}

void BBuild::interior(int a, BBuild* c1, BBuild* c2){
    this->amt = -1;
    this->b = Union(c1->b, c2->b);
    this->child[0] = c1;
    this->child[1] = c2;
    this->axis = a;
}

void BBuild::bounding(std::span<Triangle> t_span){
    //triangles should be here now
    for(Triangle &tri: t_span){
        unionBounds(tri);
    }
}

CBB BBuild::compactor(){
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
    cbb.b.bottomleft = this->b.bottomleft.copy();
    cbb.b.topright = this->b.topright.copy();
    cbb.offset = this->offset;
    cbb.amt = this->amt;
    cbb.child2 = -1;
    cbb.axis = this->axis;
    return cbb;
}

//Bounding Box linear compaction
void BBuild::compact(){
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

void SimpleBBox::compact(){
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
    cbb.b.bottomleft = this->b.bottomleft.copy();
    cbb.b.topright = this->b.topright.copy();
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
    myvec3 t_low =  (box.b.bottomleft - *position) * inv_dir;
    myvec3 t_high =  (box.b.topright - *position) * inv_dir;

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
    myvec3 t_low =  (compactBBoxes[offset].b.bottomleft - *position)*inv_dir;
    myvec3 t_high =  (compactBBoxes[offset].b.topright - *position)*inv_dir;

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
    myvec3 t_low =  (cbb.b.bottomleft - position) * inv_dir;
    myvec3 t_high =  (cbb.b.topright - position) * inv_dir;

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
    myvec3 t_low =  (compactBBoxes[0].b.bottomleft - position)*inv_dir;
    myvec3 t_high =  (compactBBoxes[0].b.topright - position)*inv_dir;

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

Bounds unionBounds(Bounds bound, Triangle obj){
    Bounds b;
    b.bottomleft.x = std::min(bound.bottomleft.x, obj.bottomleft.x);
    b.bottomleft.y = std::min(bound.bottomleft.y, obj.bottomleft.y);
    b.bottomleft.z = std::min(bound.bottomleft.z, obj.bottomleft.z);

    b.topright.x = std::max(bound.topright.x, obj.topright.x);
    b.topright.y = std::max(bound.topright.y, obj.topright.y);
    b.topright.z = std::max(bound.topright.z, obj.topright.z);
    return b;
}

Bounds Union(Bounds bound, myvec3 vec){
    Bounds b;
    b.bottomleft.x = std::min(bound.bottomleft.x, vec.x);
    b.bottomleft.y = std::min(bound.bottomleft.y, vec.y);
    b.bottomleft.z = std::min(bound.bottomleft.z, vec.z);

    b.topright.x = std::max(bound.topright.x, vec.x);
    b.topright.y = std::max(bound.topright.y, vec.y);
    b.topright.z = std::max(bound.topright.z, vec.z);
    return b;
}

//Naive splitting function
void SimpleBBox::splitRecursion(std::span<Triangle> tri_span, int offset){
    //Find which axis
    myvec3 axes = b.topright - b.bottomleft;
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
    float midpoint = (b.topright[this->axis] + b.bottomleft[this->axis]) / 2;

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
        child[0]->b.bottomleft[ax] = this->b.bottomleft[ax];
    }
    for(int ax = 0; ax < 3; ax++){
        child[0]->b.topright[ax] = this->b.topright[ax];
    }
    child[0]->b.topright[i] = midpoint;
    child[0]->splitRecursion(tri_span.subspan(0, mid), offset);

    child[1] = new SimpleBBox(this->max_obj_amt);
    for(int ax = 0; ax < 3; ax++){
        child[1]->b.bottomleft[ax] = this->b.bottomleft[ax];
    }
    for(int ax = 0; ax < 3; ax++){
        child[1]->b.topright[ax] = this->b.topright[ax];
    }
    child[1]->b.bottomleft[i] = midpoint;
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
        this->b.bottomleft.x = std::min(this->b.bottomleft.x, cent.x);
        this->b.bottomleft.y = std::min(this->b.bottomleft.y, cent.y);
        this->b.bottomleft.z = std::min(this->b.bottomleft.z, cent.z);
    
        this->b.topright.x = std::max(this->b.topright.x, cent.x);
        this->b.topright.y = std::max(this->b.topright.y, cent.y);
        this->b.topright.z = std::max(this->b.topright.z, cent.z);
    }
}

void SimpleBBox::bounding(std::span<Triangle> tri_span){
    //triangles should be here now
    for(Triangle &tri: tri_span){
        unionBounds(tri);
    }
}

//too much hassle, let's just call compactor once
void MortonBBox::split(){
    return;
}

BBuild* emit(BBuild *&BNode, mortonObj* morton, int amt, int* totalNodes, 
            std::vector<Triangle> &orderedTriangles, std::atomic<int> *orderedOffset, int bitindex){
    //leaf
    if (bitindex == -1 || amt < maxPrims){
        ++*totalNodes;
        BBuild* node = BNode++;
        Bounds bound;
        bound.bottomleft = myvec3(FLT_MAX, FLT_MAX,FLT_MAX);
        bound.topright = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        int firstOffset = orderedOffset->fetch_add(amt);
        for (int i = 0; i<amt; i++){
            int triIndex = morton[i].offset;
            orderedTriangles[firstOffset+i] = TriangleList[triIndex];
            bound = unionBounds(bound, TriangleList[triIndex]);
        }

        node->leaf(firstOffset, amt, bound.bottomleft, bound.topright);
        return node;
    }
    //not leaf
    else{
        //Get the mask
        int mask = 1 << bitindex;
        //If there's no need to split this node
        if((morton[0].morton & mask) == (morton[amt-1].morton & mask))
            return emit(BNode, morton, amt, 
                totalNodes, orderedTriangles, orderedOffset, bitindex-1);

        //Find interval through...WHAT CODE IS THIS HUH???????????????????
        int splitOffset = FindInterval(amt, [&](int index) {
            return ((morton[0].morton & mask) == (morton[index].morton & mask));
        });
        ++splitOffset;

        //Make the child nodes
        (*totalNodes)++;
        BBuild* node = BNode++;
        BBuild* lbvh[2] = {
            emit(BNode, morton, splitOffset, 
                totalNodes, orderedTriangles, orderedOffset, bitindex-1),
            emit(BNode, &morton[splitOffset], amt-splitOffset, 
                totalNodes, orderedTriangles, orderedOffset, bitindex-1)
        };
        int axis = bitindex%3;
        node->interior(axis, lbvh[0], lbvh[1]);
        return node;
    }
}

float Bounds::SurfaceArea(){
    myvec3 d = this->topright - this->bottomleft;
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

BBuild* BuildUpperSAH(std::vector<BBuild *> finishedTrees, int start, int end){
    int nNodes = end - start;
    // std::cout<<nNodes<<" "<<end<<" "<<start<<"\n";
    // std::cout<<finishedTrees.size()<<"\n";
    if (nNodes == 1)
        return finishedTrees[start];
    BBuild *node = new BBuild;

    // Compute bounds of all nodes under this HLBVH node
    Bounds bounds;
    bounds.bottomleft = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    bounds.topright = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i = start; i < end; ++i)
        bounds = Union(bounds, finishedTrees[i]->b);

    // Compute bound of HLBVH node centroids, choose split dimension _dim_
    Bounds centroidBounds;
    centroidBounds.bottomleft = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    centroidBounds.topright = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (int i = start; i < end; ++i) {
        myvec3 centroid =
            (finishedTrees[i]->b.bottomleft + finishedTrees[i]->b.topright) * 0.5f;
        centroidBounds = Union(centroidBounds, centroid);
    }

    myvec3 axes = centroidBounds.topright - centroidBounds.bottomleft;
    int dim = 0;
    if (axes.y > axes.x){
        if (axes.z > axes.y) {
            dim = 2;
        }
        else dim = 1;
    }
    else if (axes.z > axes.x){
        dim = 2;
    }

    // Allocate _BVHSplitBucket_ for SAH partition buckets
    constexpr int nBuckets = 12;
    struct BVHSplitBucket {
        int count = 0;
        Bounds bounds;
    };
    BVHSplitBucket buckets[nBuckets];

    // Initialize _BVHSplitBucket_ for HLBVH SAH partition buckets
    for (int i = start; i < end; ++i) {
        float centroid = (finishedTrees[i]->b.bottomleft[dim] + finishedTrees[i]->b.topright[dim]) * 0.5f;
        int b = nBuckets * ((centroid - centroidBounds.bottomleft[dim]) / (centroidBounds.topright[dim] - centroidBounds.bottomleft[dim]));
        if (b == nBuckets)
            b = nBuckets - 1;
        buckets[b].count++;
        buckets[b].bounds = Union(buckets[b].bounds, finishedTrees[i]->b);
    }

    // Compute costs for splitting after each bucket
    float cost[nBuckets - 1];
    for (int i = 0; i < nBuckets - 1; ++i) {
        Bounds b0, b1;
        int count0 = 0, count1 = 0;
        for (int j = 0; j <= i; ++j) {
            b0 = Union(b0, buckets[j].bounds);
            count0 += buckets[j].count;
        }
        for (int j = i + 1; j < nBuckets; ++j) {
            b1 = Union(b1, buckets[j].bounds);
            count1 += buckets[j].count;
        }
        cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) /
        bounds.SurfaceArea();
    }

    // Find bucket to split at that minimizes SAH metric
    float minCost = cost[0];
    int minCostSplitBucket = 0;
    for (int i = 1; i < nBuckets - 1; ++i) {
        if (cost[i] < minCost) {
            minCost = cost[i];
            minCostSplitBucket = i;
        }
    }

    // Split nodes and create interior HLBVH SAH node
    BBuild **pmid = std::partition(
        &finishedTrees[start], &finishedTrees[end - 1] + 1, [=](const BBuild *node) {
            float centroid = (node->b.bottomleft[dim] + node->b.topright[dim]) * 0.5f;
            int b = nBuckets * ((centroid - centroidBounds.bottomleft[dim]) / (centroidBounds.topright[dim] - centroidBounds.bottomleft[dim]));
            if (b == nBuckets)
                b = nBuckets - 1;
        return b <= minCostSplitBucket;
    });
    int mid = pmid - &finishedTrees[0];
    node->interior(dim,
        BuildUpperSAH(finishedTrees, start, mid),
        BuildUpperSAH(finishedTrees, mid, end));
    return node;
}

void Printer(BBuild* bb, int level, int side){
    std::cout<<"level: "<<level<<"\n";
    std::cout<<"side: "<<side<<"\n";
    printvec3(bb->b.bottomleft);
    std::cout<<"\n";
    printvec3(bb->b.topright);
    std::cout<<"\n";

    if (bb->amt<1){
        std::cout<<"\n";
        Printer(bb->child[0], level+1, 0);
        Printer(bb->child[1], level+1, 1);
    }
    else{
        std::cout<<bb->offset<<" "<<bb->offset+bb->amt<<"\n";
        std::cout<<"leaf\n\n";
    }
}

void MortonBBox::compact(){
    //The base treelet to compact
    std::vector<LBVHTreelet> treelets;
    //find the min/max
    myvec3 min = myvec3(FLT_MAX, FLT_MAX, FLT_MAX);
    myvec3 max = myvec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
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

    //radix sort, yoinked from the book
    std::vector<mortonObj> tempVector(mObj.size());
    constexpr int bitsPerPass = 6;
    constexpr int nBits = 30;
    constexpr int nPasses = nBits / bitsPerPass;
    for (int pass = 0; pass < nPasses; ++pass) {
        //lowest bit in this pass
        int lowBit = pass * bitsPerPass;
        // in/out vector
        std::vector<mortonObj> &in = (pass & 1) ? tempVector : mObj;
        std::vector<mortonObj> &out = (pass & 1) ? mObj : tempVector;
        //number of items in each bucket
        constexpr int nBuckets = 1 << bitsPerPass;
        int bucketCount[nBuckets] = { 0 };
        constexpr int bitMask = (1 << bitsPerPass) - 1;
        for (const mortonObj &mp : in) {
            int bucket = (mp.morton >> lowBit) & bitMask;
            ++bucketCount[bucket];
        }
        // for(int buck: bucketCount){
        //     std::cout<<buck<<"\n";
        // }
        // std::cout<<"\n";

        //the index for each item
        int outIndex[nBuckets];
        outIndex[0] = 0;
        for (int i = 1; i < nBuckets; ++i)
            outIndex[i] = outIndex[i - 1] + bucketCount[i - 1];
            
        for (const mortonObj &mp : in) {
            int bucket = (mp.morton >> lowBit) & bitMask;
            out[outIndex[bucket]++] = mp;
        }
    }
    if (nPasses & 1) std::swap(mObj, tempVector);

    //start making the LBVH
    //vector of all LBVH treelet
    for (size_t start = 0, end = 1; end <= mObj.size(); ++end){
        //Mask for clustering the 2^12 clusters
        uint32_t mask = 0b00111111111111000000000000000000;
        //clusters the morton codes with the same first 12 bits
        if (end == (int) mObj.size() || ((mObj[start].morton & mask) != (mObj[end].morton & mask))){
            size_t amt = end-start;
            BBuild *nodes = (BBuild*) malloc(sizeof(BBuild));
            treelets.push_back({start, amt, nodes});
            
            start = end;
        }
    }
    std::atomic<int> orderedOffset(0);
    std::vector<Triangle> orderedTriangles(TriangleList.size());
    //craft the builder for each treelet
    #pragma omp parallel for
    for(int i = 0; i<treelets.size(); i++){
        int nodesCreated = 0;
        const int firstBit = 17; // 29-12
        LBVHTreelet &tr = treelets[i];

        //build the treelet 
        tr.nodes = emit(tr.nodes, &mObj[tr.start], tr.prim_amt, 
                        &nodesCreated, orderedTriangles, &orderedOffset, firstBit);
    }

    //After emitting, make the BVH
    std::vector<BBuild*> finishedTrees;
    finishedTrees.reserve(treelets.size());
    std::swap(orderedTriangles, TriangleList);
    for (LBVHTreelet &treelet : treelets)
        finishedTrees.push_back(treelet.nodes);
    //get the final bbuild
    BBuild* end_build = BuildUpperSAH(finishedTrees, 0, finishedTrees.size());
    end_build->compact();
    // Printer(end_build, 0, 0);
}