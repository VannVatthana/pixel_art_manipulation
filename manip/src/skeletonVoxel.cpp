#include "skeletonVoxel.hpp"

using namespace cgp;

// Bone Class
Bone::Bone(const std::string& boneName, const vec3& startJoint, const vec3& endJoint)
{
    name = boneName;
    globalTransformation = initializeGlobalTransformation(startJoint,endJoint);//initializeGlobalTransformation(startJoint,endJoint);
}
mat4 Bone::initializeGlobalTransformation(const vec3& start, const vec3& end)
{
    vec3 translation = start;
    mat4 translationMatrix = mat4::build_translation(translation);

    vec3 direction = normalize(end-start);
    vec3 upVector = vec3{0.0f,1.0f,0.0f};

    vec3 rightVector = cross(upVector,direction);
    matrix_stack<float, 4,4> rotationMatrix (vec4{rightVector,0.0f}, vec4{upVector,0.0f}, vec4{-direction,0.0f}, vec4{0,0,0,1});
    transpose(rotationMatrix);

    mat4 globalTransformation = translationMatrix * rotationMatrix;
    return globalTransformation;
}

void Bone::updateGlobalTransformation(const mat4& parentOffset = mat4::build_identity()){
    globalTransformation = parentOffset * globalTransformation;
    //for (auto& child : children)
    //    child.updateglobalTransformation(this->globalTransformation);
}

// For Voxel Class
void Voxel::assignMeatToBone(const int boneID, const vec3& in, const vec3& out)
{
    std::vector<int> meatIDs; // store only IDs of Voxel Cubes
    // Suppose that x_in < x_out, y_in < y_out, z_in < z_out
    for(int i = 0; i < v.size(); i++)
    {
        bool biggerThanIn = (v[i].x >= in.x) && (v[i].y >= in.y) && (v[i].z >= in.z);
        bool smallerThanOut = (v[i].x <= out.x) && (v[i].y <= out.y) && (v[i].z <= out.z);
        if(biggerThanIn && smallerThanOut)
            meatIDs.push_back(i);
        
    }
    skeleton[boneID].associatedCubes =  meatIDs;
}

void Voxel::updateCubesWithBone(const int boneID)//const Bone* bone
{
    for (int i = 0; i < skeleton[boneID].associatedCubes.size(); i++)
    {
        vec4 originalPos = vec4{v[i],1.0f};
        vec4 transformedPos = skeleton[boneID].globalTransformation * originalPos;
        
        v[i] = vec3{int(transformedPos.x), int(transformedPos.y), int(transformedPos.z)};
    }
}