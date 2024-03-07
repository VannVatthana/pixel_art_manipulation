#pragma once

#include "cgp/cgp.hpp"
#include <vector> 

using namespace cgp;
struct SpringVoxel
{
   std::vector<vec3> v;   // Coordinates for each cube
   std::vector<vec3> rgb; // Color for each cube
   
   std::vector<std::vector<int>> neighbors; // need neighbors to connect them with springs
   std::vector<vec3> forces;
   std::vector<vec3> velocities;
   std::vector<vec3> prevVertices;
   std::vector<std::vector<float>> restLength;

   //std::vector<mesh_drawable> voxel_mesh;
   //bool isRigging = false;       // when the voxel is being rigged
   int angle;

   /** Methods **/
   void initialize();
   std::tuple<std::vector<int>, std::vector<float>> getNeighbors(int id); // get indices of neighbors of this voxel;
   
   void apply_spring_forces(); 
};  


