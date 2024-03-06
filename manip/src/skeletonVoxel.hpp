#pragma once

#include "cgp/cgp.hpp"
#include <vector> 

using namespace cgp;

struct Bone{
    std::string name;
    //vec3 startJoint;
    //vec3 endJoint;
    mat4 globalTransformation;
    //std::vector<Bone> children;        // Children Bone
    std::vector<int> associatedCubes;  //which cubes are associated with this bone

    Bone(const std::string& boneName, const vec3& startJoint, const vec3& endJoint);
    mat4 initializeGlobalTransformation(const vec3& startJ, const vec3& endJ);

    void updateGlobalTransformation(const mat4& parentOffset);
};

struct Voxel{
    std::vector<vec3> v;     // all the cubes that form the voxel
    std::vector<vec3> rgb;   // RGB color for each cube
    std::vector<Bone> skeleton; // All bones of the voxel model
    //Bone* skeleton;

    void assignMeatToBone(const int boneID, const vec3& in, const vec3& out); // Assign cube in a region as a part to move with that bone
    void updateCubesWithBone(const int boneID);     //const Bone* bone

};