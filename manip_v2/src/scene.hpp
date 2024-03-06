#pragma once


#include "cgp/cgp.hpp"
#include "environment.hpp"

#include "loadImg.cpp"

using cgp::mesh_drawable;
using cgp::curve_drawable;
using cgp::mesh;
using cgp::numarray;
using cgp::vec3;



struct gui_parameters {
	bool display_frame = true;
	bool sketch_mode = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	SpringVoxel voxel_model;
	std::vector<mesh_drawable> voxel_mesh;
	
	bool isRigging = false;
	

	vec2 initial_mouse_pos;
	int deforming_voxel_idx;
	
	// PNG 
	Voxel3D vox_from_png; 
	std::vector<mesh_drawable> png_mesh;
	int selectButton= 0;
	
	//   Each new stroke (continuous click+motion of the mouse) is a new element of the buffer
	//cgp::numarray<cgp::curve_drawable_dynamic_extend> sketch_drawable;
	cgp::curve_drawable_dynamic_extend sketch_drawable;
	vec3  curveStartPoint,curveEndPoint;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

};





