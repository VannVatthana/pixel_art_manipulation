#include "springVoxel.hpp"


void SpringVoxel::initialize()
{
    for (int i = 0; i < this->v.size(); i++)
    {
        prevVertices.push_back(v[i]);
        std::tuple<std::vector<int>, std::vector<float>>  neighbWithRest = getNeighbors(i);
        neighbors.push_back(get<0>(neighbWithRest));
        restLength.push_back(get<1>(neighbWithRest));

        velocities.push_back(vec3{0.0f, 0.0f, 0.0f});
        forces.push_back(vec3{0.0f, 0.0f, 0.0f});
    }
}

std::tuple<std::vector<int>, std::vector<float>> SpringVoxel::getNeighbors(int id)
{
    std::vector<int> neighb;
    std::vector<float> restLen; 
    for (int i = 0; i < v.size(); i++ ){
        if(i != id){
            vec3 vox = v[id];
            if ((v[i].x <= vox.x + 1 && v[i].x >= vox.x - 1) && (v[i].y <= vox.y + 1 && v[i].y >= vox.y - 1) && (v[i].z <= vox.z + 1 && v[i].z >= vox.z - 1))
            {
                neighb.push_back(i);
                restLen.push_back(norm(vox - v[i]));
            }
        }
    }
    return std::tuple<std::vector<int>, std::vector<float>>(neighb, restLen);
}

void SpringVoxel::apply_spring_forces()
{
    // Calculate the spring force between each voxel and its neighbors
    for (int i = 0; i < v.size(); i++)
    {
        for (int k = 0; k < neighbors[i].size(); k++)
        {
            int j = neighbors[i][k];
            vec3 neighbor_pos = v[j];
            vec3 displacement = -(neighbor_pos - v[i]);
            float rest = restLength[i][k];

            float distance = norm(displacement);
            float spring_cte = 1.0f;
            float damping_cte = 1.0f;

            // Calculate the spring force
            vec3 spring_force = -spring_cte * (distance - rest) * normalize(displacement);

            // Calculate the damping force
            vec3 damping_force = -damping_cte * velocities[i];

            // Add the spring force and damping force to the total force
            forces[i] += spring_force + damping_force;
        }   
    }    
    for (int i = 0; i < v.size(); i++)
    {
        // Apply the force to the voxel's velocity
        velocities[i] += forces[i];
        v[i] += velocities[i]*0.1f;
        // reset to 0
        velocities[i] = vec3{0.0f,0.0f,0.0f};
        forces[i] = vec3{0.0f,0.0f,0.0f};
    }

}

// V1 //
/*void SpringVoxel::RotateVec3(vec3& pos, int angle, const vec3& axis, const vec3& origin){

    pos -= origin;    
    float rad = angle * (M_PI / 180.0);
    float sin_ = sin(rad / 2.0f);
    float cos_ = cos(rad / 2.0f);

    mat3 R{cos_, 0, sin_,
            0, 1, 0,
            -sin_,0,cos_};
    vec3 newPos{round(cos_*pos.x + sin_*pos.z), round(pos.y), round(-sin_* pos.x +cos_*pos.z)};


    // Update the rotated position
    pos = newPos + origin;
}
void SpringVoxel::RotateHandlePoints(std::vector<vec3>& fixedPoints, std::vector<vec3>& handlePoints, int angle, const vec3& axis){
    //vec3 handle_center{0,0,0}; 
    vec3 fixed_center{0,0,0};

    for (vec3 fix : fixed_points)
        fixed_center += fix;
    fixed_center = fixed_center / fixed_points.size();
    
    std::cout<< "In" << handle_points[4]<<std::endl;
    for (int i = 0;i< handle_points.size(); i++)
    {
        RotateVec3(handle_points[i], angle, axis, fixed_center);
    }
    std::cout<< "Out" << handle_points[4]<<std::endl;
}*/

