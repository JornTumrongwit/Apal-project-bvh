/*****************************************************************************/
/* File reader from CSE167 by Ravi Ramamoorthi                               */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  
  

// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 
  
/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <cmath>
#include "Transform.h" 

using namespace std;
#include "objects.h"
#include "matvec.h"
#include "readfile.h"
#include "variables.h"

// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mymat4> &transfstack, float* values) 
{
    mymat4 transform = transfstack.top(); 
    myvec4 valvec = myvec4(values[0],values[1],values[2],values[3]); 
    myvec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mymat4 & M, stack<mymat4> &transfstack) 
{
    mymat4 &T = transfstack.top(); 
    T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values) 
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i]; 
        if (s.fail()) {
            clog << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
    }
    return true; 
}

bool readind(stringstream& s, const int numvals, int* val)
{
    for (int i = 0; i < numvals; i++) {
        s >> val[i];
        if (s.fail()) {
            clog << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}

void readfile(const char* filename)
{
    string str, cmd;
    ifstream in;
    in.open(filename);
    if (in.is_open()) {

        // I need to implement a matrix stack to store transforms.  
        // This is done using standard STL Templates 
        stack <mymat4> transfstack;
        stack <mymat4> invtransfstack;
        transfstack.push(mymat4(1.0));  // identity
        invtransfstack.push(mymat4(1.0));  // identity

        getline(in, str);
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines 

                stringstream s(str);
                s >> cmd;
                int i;
                float values[10]; // Position and color for light, colors for others
                // Up to 10 params for cameras.  
                int val[3];
                bool validinput; // Validity of input 

                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "point") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE. 
                        // Note that values[0...7] shows the read in values 
                        // Make use of lightposn[] and lightcolor[] arrays in variables.h
                        // Those arrays can then be used in display too. 
                        // DONE
                        myvec4 lightpos = myvec4(values[0], values[1], values[2], 1);
                        lightposn.push_back(lightpos);
                        myvec3 lightcol = myvec3(values[3], values[4], values[5]);
                        lightcolor.push_back(lightcol);
                        ++numused;
                    }
                }

                else if (cmd == "directional") {
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE. 
                        // Note that values[0...7] shows the read in values 
                        // Make use of lightposn[] and lightcolor[] arrays in variables.h
                        // Those arrays can then be used in display too. 
                        // DONE
                        myvec4 lightpos = myvec4(values[0], values[1], values[2], 0);
                        lightposn.push_back(lightpos);
                        myvec3 lightcol = myvec3(values[3], values[4], values[5]);
                        lightcolor.push_back(lightcol);
                        ++numused;
                    }
                }

                //maximum depth, default is 5
                else if (cmd == "maxdepth") {
                    validinput = readvals(s, 1, values); // Position/color for lts.
                    if (validinput) {
                        reflectdepth = values[0];
                    }
                }

                //output file name
                else if (cmd == "output") {
                    s >> outname;
                }


                // Material Commands 
                // Ambient, diffuse, specular, shininess properties for each object.
                // Filling this in is pretty straightforward, so I've left it in 
                // the skeleton, also as a hint of how to do the more complex ones.
                // Note that no transforms/stacks are applied to the colors. 

                else if (cmd == "ambient") {
                    validinput = readvals(s, 3, values); // colors 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            ambient[i] = values[i];
                        }
                        ambient[3] = 1;
                    }
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            diffuse[i] = values[i];
                        }
                    }
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            specular[i] = values[i];
                        }
                    }
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            emission[i] = values[i];
                        }
                    }
                }
                else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            attenuation[i] = values[i];
                        }
                    }
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values);
                    if (validinput) {
                        shininess = values[0];
                    }
                }
                else if (cmd == "size") {
                    validinput = readvals(s, 2, values);
                    if (validinput) {
                        w = (int)values[0]; h = (int)values[1];
                    }
                }
                else if (cmd == "camera") {
                    validinput = readvals(s, 10, values); // 10 values eye cen up fov
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE
                        // Use all of values[0...9]
                        // You may need to use the upvector fn in Transform.cpp
                        // to set up correctly. 
                        // Set eyeinit upinit center fovy in variables.h 
                        // DOING
                        for (i = 0; i < 3; i++) {
                            eyeinit[i] = values[i];
                        }
                        for (i = 0; i < 3; i++) {
                            center[i] = values[i + 3];
                        }
                        myvec3 up = myvec3(values[6], values[7], values[8]);
                        wcam = normalize(eyeinit - center);
                        ucam = normalize(cross(up, wcam));
                        vcam = cross(wcam, ucam);
                        upinit = vcam;
                        fovy = values[9];
                        tany = tan(fovy*pi/360);
                        tanx = (w / 2) * tany / (h / 2);
                        fovx = atan(tanx) * 360/pi;
                    }
                }
                
                else if (cmd == "sphere") {
                    validinput = readvals(s, 4, values);
                    if (validinput) {
                        Sphere sph;
                        sph.position = myvec3(values[0], values[1], values[2]);
                        sph.radius = values[3];
                        for (int i = 0; i < 3; i++) {
                            sph.ambient[i] = ambient[i];
                            sph.specular[i] = specular[i];
                            sph.diffuse[i] = diffuse[i];
                            sph.emission[i] = emission[i];
                        }
                        sph.shininess = shininess;
                        sph.transform = transfstack.top();
                        sph.inv_transform = invtransfstack.top();
                        sph.poly_type = SPHERE;
                        sph.Bound();
                        SphereList.push_back(sph);
                    }
                }

                else if (cmd == "maxverts") {
                    size_t value;
                    s >> value;
                }

                else if (cmd == "vertex") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        myvec4 vert = myvec4(values[0], values[1], values[2], 1);
                        vertices.push_back(vert);
                    }
                }

                else if (cmd == "tri") {
                    validinput = readind(s, 3, val);
                    if (validinput) {
                        mymat4 T = transfstack.top();
                        Triangle tri;
                        tri.vertA = T * vertices[val[0]];
                        tri.vertB = T * vertices[val[1]];
                        tri.vertC = T * vertices[val[2]];
                        myvec4 BminusA = tri.vertB - tri.vertA;
                        myvec4 CminusA = tri.vertC - tri.vertA;
                        tri.normal = normalize(cross(myvec3(BminusA[0], BminusA[1], BminusA[2]), myvec3(CminusA[0], CminusA[1], CminusA[2])));
                        for (int i = 0; i < 3; i++) {
                            tri.ambient[i] = ambient[i];
                            tri.specular[i] = specular[i];
                            tri.diffuse[i] = diffuse[i];
                            tri.emission[i] = emission[i];
                        }
                        tri.shininess = shininess;
                        tri.poly_type = TRIANGLE;
                        tri.Bound();
                        TriangleList.push_back(tri);
                    }
                }

                else if (cmd == "translate") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE.  
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file. 
                        // Also keep in mind what order your matrix is!
                       mymat4& T = transfstack.top(); 
                       mymat4& invT = invtransfstack.top();
                       T = T * Transform::translate(values[0], values[1], values[2]);
                       invT = Transform::translate(-values[0], -values[1], -values[2]) * invT;
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE.  
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file.  
                        // Also keep in mind what order your matrix is!
                        mymat4& T = transfstack.top();
                        mymat4& invT = invtransfstack.top();
                        T = T * Transform::scale(values[0], values[1], values[2]);
                        invT = Transform::scale(1/values[0], 1/values[1], 1/values[2]) * invT;
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s, 4, values);
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE. 
                        // values[0..2] are the axis, values[3] is the angle.  
                        // You may want to normalize the axis (or in Transform::rotate)
                        // See how the stack is affected, as above.  
                        // Note that rotate returns a mat3. 
                        // Also keep in mind what order your matrix is!
                        mymat4& T = transfstack.top();
                        mymat4& invT = invtransfstack.top();
                        mymat3 rotation = Transform::rotate(values[3], myvec3(values[0], values[1], values[2]));
                        mymat3 invrotation = Transform::rotate(-values[3], myvec3(values[0], values[1], values[2]));
                        T = T * rotation;
                        invT = invrotation * invT;
                    }
                }

                // I include the basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    invtransfstack.push(invtransfstack.top());
                    transfstack.push(transfstack.top());
                    mymat4& T = transfstack.top();
                    mymat4& invT = invtransfstack.top();
                }
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements.  Cannot Pop\n";
                    }
                    else {
                        transfstack.pop();
                        invtransfstack.pop();
                    }
                }

                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n";
                }
            }
            getline(in, str);
        }
    }
    else {
        cerr << "Unable to Open Input Data File " << filename << "\n";
        throw 2;
    }
}
