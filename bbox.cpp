#include "bbox.h"
#include "variables.h"
#include <iostream>

//Naive splitting function
void SimpleBBox::splitRecursion(AXIS a, std::span<Triangle> tri_span, int offset){
    //if leaf is small enough, return
    if (tri_span.size() <= max_obj_amt){
        this->offset = offset;
        this->amt = tri_span.size();
        std::cout<<"Got box of "<<this->amt << " items at offset " << this->offset<<"\n";
        return;
    }
    
    int i = 0;
    if(a == Y) i = 1;
    if(a == Z) i = 2;
    //find midpoint
    float midpoint = (topright[i] + bottomleft[i]) / 2;

    //case to run
    //Run mid split first, then centroid balancing
    int splitmethod = 1;
    int mid;
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

    std::cout<<"Splitting "<<offset<<" of size " << tri_span.size() << " with mid " << mid << "\n";

    child[0] = new SimpleBBox(this->max_obj_amt);
    child[0]->splitRecursion(a, tri_span.subspan(0, mid), offset);

    child[1] = new SimpleBBox(this->max_obj_amt);
    child[0]->splitRecursion(a, tri_span.subspan(mid), offset+mid);
}

void SimpleBBox::split(){
    //First, find which axis
    myvec3 axes = topright - bottomleft;
    AXIS to_split = X;
    if (axes.y > axes.x){
        if (axes.z > axes.y) {
            to_split = Z;
        }
        else to_split = Y;
    }
    else if (axes.z > axes.x){
        to_split = Z;
    }

    std::span<Triangle> tri_span(TriangleList); 
    //call the recursive function
    this->splitRecursion(to_split, tri_span, 0);
}