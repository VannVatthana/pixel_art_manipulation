#include "third_party/src/lodepng/lodepng.h"

#include <iostream>
#include <vector>

#include "cgp/cgp.hpp"
#include <vector> 
#include "springVoxel.hpp"
#include "skeletonVoxel.hpp"
using namespace cgp;

struct Image2D
{
    unsigned width, height;
    std::vector<std::vector<vec4>> rgba;
    
    std::vector<std::vector<vec4>> updated_rgba; // for after deforming the voxel

    void png2rgba(const char* fileName)
    {
        std::vector<unsigned char> image;
        unsigned error = lodepng::decode(image, width, height, fileName);

        //if there's an error, display it
        if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

        rgba = std::vector<std::vector<vec4>>(height, std::vector<vec4>(width));

        // Convert the image data to floats
        float* floatImage = new float[width * height * 4];
        for (unsigned int i = 0; i < width * height * 4; i++) {
            floatImage[i] = (float)image[i] / 255.0f;
        }

        // Access the r, g, b, alpha values
        for (unsigned int y = 0; y < height; y++) {
            for (unsigned int x = 0; x < width; x++) {
                unsigned int index = 4 * (y * width + x);
                rgba[y][x] = vec4{floatImage[index + 0], floatImage[index + 1], floatImage[index + 2], floatImage[index + 3]}; 
            }
        }
        // Free the memory used by the image data
        delete[] floatImage;
    }

    void updatedPng(const char* updatedFileName)
    {
        std::vector<unsigned char> updated_image;

        for (unsigned int y = 0; y < height; y++)
            for (unsigned int x = 0; x < width; x++)
                for(unsigned int i = 0; i < 4; i++)
                    updated_image.push_back(static_cast<unsigned char>(updated_rgba[y][x][i]*255.0f)); 

        unsigned error = lodepng::encode(updatedFileName, updated_image,width, height);

         // Check for errors
        if (error) 
            std::cerr << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
        
};
struct Voxel3D
{
    Image2D front;
    Image2D back;
    Image2D side;
    Image2D top;
    //voxel_structure voxel;
    Voxel voxel;
    
    // Suppose that all views have the same resolution
    void buildVoxel3D()
    {
        // voxel.x = front.width, voxel.y = imageSize = side.width, voxel.z = front.height //
        
        // Build voxel shape with front and back view
        int imageSize = side.width;

        for (int xvox = 0; xvox < imageSize; xvox++) // front.width
            for(int zvox = 0; zvox < imageSize; zvox++) // front.height
                if (front.rgba[zvox][xvox].w == 1)  
                {
                    for(int yvox = 0; yvox < int(imageSize/2); yvox++) {
                        if (side.rgba[zvox][yvox].w == 1 && top.rgba[yvox][xvox].w == 1) { // Remove the excess cubes using Side and top view
                            voxel.v.push_back(vec3{float(xvox), float(yvox), float(-zvox+imageSize-1)});
                            voxel.rgb.push_back(vec3{front.rgba[zvox][xvox].x, front.rgba[zvox][xvox].y, front.rgba[zvox][xvox].z});
                        }
                    }
                    
                    for(int yvox = int(imageSize/2); yvox < imageSize; yvox++) {
                        if (side.rgba[zvox][yvox].w == 1 && top.rgba[yvox][xvox].w == 1) {
                            voxel.v.push_back(vec3{float(xvox), float(yvox), float(-zvox+imageSize-1)});
                            voxel.rgb.push_back(vec3{back.rgba[zvox][xvox].x, back.rgba[zvox][xvox].y, back.rgba[zvox][xvox].z});   
                        }
                    }  
                }
    }   

    void reencode2img()
    {
        // Reset the rgba data
        int img_size = front.height;
        front.updated_rgba.clear();
        front.updated_rgba = std::vector<std::vector<vec4>>(img_size, std::vector<vec4>(img_size));

        back.updated_rgba.clear();
        back.updated_rgba = std::vector<std::vector<vec4>>(img_size, std::vector<vec4>(img_size));

        side.updated_rgba.clear();
        side.updated_rgba = std::vector<std::vector<vec4>>(img_size, std::vector<vec4>(img_size));
        

        for(int i = 0; i < voxel.v.size(); i++)
        {
            vec3 p_front = voxel.v[i];
            if(front.updated_rgba[img_size-p_front.z-1][p_front.x].w == 0) // We only need to know whether alpha = 0 or 1
                front.updated_rgba[img_size-p_front.z-1][p_front.x] = vec4{voxel.rgb[i],1.0f};  

            vec3 p_back = voxel.v[voxel.v.size()- i];// + offset_vector;
            if(back.updated_rgba[img_size-p_back.z-1][img_size - p_back.x-1].w == 0) // We only need to know whether alpha = 0 or 1
                back.updated_rgba[img_size-p_back.z-1][img_size - p_back.x-1] = vec4{voxel.rgb[voxel.v.size()-i-1],1.0f};  

            if(side.updated_rgba[img_size-p_front.z-1][p_front.y].w == 0) // We only need to know whether alpha = 0 or 1
                side.updated_rgba[img_size-p_front.z-1][p_front.y] = vec4{voxel.rgb[i],1.0f};   

        }
        
        // Reencode data to png
        front.updatedPng("../assets/results/newFront.png");
        back.updatedPng("../assets/results/newBack.png");
        side.updatedPng("../assets/results/newSide.png");

        std::cout<<"Export complete!"<<std::endl;
    }
};