#include "skeletonVoxel.hpp"

using namespace cgp;

// Bone Class
Bone::Bone(const std::string& boneName, const vec3& start, const vec3& end)//, const vec3& forward)
{
    name = boneName;
    startJoint = start;
    endJoint = end;
    //forwardPoint = forward;
    calculateGlobalTransformation(startJoint,endJoint);
}
void Bone::calculateGlobalTransformation(const vec3& start, const vec3& end)//, const vec3& forward)
{
    vec3 translation = start;
    mat4 translationMatrix = mat4::build_translation(translation);

    vec3 z_axis = normalize(end-start);
    vec3 onevector{1,1,1};
    vec3 x_axis = normalize(cross(onevector,z_axis));
    vec3 y_axis = normalize(cross(z_axis,x_axis)); 
    
    //float rotation_angle = acos(dot(vec3{0,0,1},z_axis));

    matrix_stack<float,4,4> rotationMatrix(vec4{x_axis,0.f},vec4{y_axis,0.f},vec4{z_axis,0.f},vec4{0.f,0.f,0.f,1.f});
    rotationMatrix = transpose(rotationMatrix);
    globalTransformation = translationMatrix * rotationMatrix;
}

void Bone::updateGlobalTransformation(const mat4& parentOffset = mat4::build_identity()){
    globalTransformation = parentOffset * globalTransformation;
    for (auto& child : children)
        child->updateGlobalTransformation(this->globalTransformation);
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
    skeleton[boneID]->associatedCubes =  meatIDs;
}

void Voxel::calculateLocalPositions(const int boneID)
{
    for (int asso_cube : skeleton[boneID]->associatedCubes)
    {
        vec4 local_position = inverse(skeleton[boneID]->globalTransformation) * vec4{v[asso_cube],1.0f};
        skeleton[boneID]->localPos.push_back(vec3{local_position.x, local_position.y, local_position.z});
    }
}

void Voxel::updateCubesWithBone(const int boneID)//const Bone* bone
{
    for (int i = 0; i < skeleton[boneID]->associatedCubes.size(); i++)
    {
        vec4 local_position = vec4{skeleton[boneID]->localPos[i],1.0f};
        vec4 newGlobalPos = skeleton[boneID]->globalTransformation * local_position;
        
        v[skeleton[boneID]->associatedCubes[i]] = vec3{round(newGlobalPos.x), round(newGlobalPos.y), round(newGlobalPos.z)};
    }
}