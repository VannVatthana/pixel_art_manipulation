#include "scene.hpp"


// Test scene to check correct compilation and run
//  The code should display a rotating cube and a textured quad on top of a circular gfloor.

using namespace cgp;

// Compute a 3D position of a 2D position given by its screen coordinates for perspective projection
vec3 unproject(camera_projection_perspective const& P, mat4 const& camera_view_inverse, vec2 const& p_screen)
{
	// Simple un-project assuming that the viewpoint is an orthogonal projection
	vec4 const p_proj = camera_view_inverse * P.matrix_inverse() *  vec4(p_screen, 0.5f, 1.0f);
	return p_proj.xyz()/p_proj.w;
}

void scene_structure::initialize()
{
	// Set the behavior of the camera and its initial position
	camera_control.initialize(inputs, window);
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 24.0f, 16.0f,16.0f }, { 15,15,15 }, { 0,0,1 });

	// Create a visual frame representing the coordinate system
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Sample arm
	/*voxel_model.v = {{-1,1,0}, {0,1,0}, {1,1,0}, {-1,0,0}, {0,0,0}, {1,0,0}, {-1,-1,0}, {0,-1,0}, {1,-1,0},
					{-1,1,1}, {0,1,1}, {1,1,1}, {-1,0,1}, {0,0,1}, {1,0,1}, {-1,-1,1}, {0,-1,1}, {1,-1,1},
					{-1,1,2}, {0,1,2}, {1,1,2}, {-1,0,2}, {0,0,2}, {1,0,2}, {-1,-1,2}, {0,-1,2}, {1,-1,2},
					{-1,1,3}, {0,1,3}, {1,1,3}, {-1,0,3}, {0,0,3}, {1,0,3}, {-1,-1,3}, {0,-1,3}, {1,-1,3},
					{-1,1,4}, {0,1,4}, {1,1,4}, {-1,0,4}, {0,0,4}, {1,0,4}, {-1,-1,4}, {0,-1,4}, {1,-1,4},
					{-1,1,5}, {0,1,5}, {1,1,5}, {-1,0,5}, {0,0,5}, {1,0,5}, {-1,-1,5}, {0,-1,5}, {1,-1,5},
					{-1,1,6}, {0,1,6}, {1,1,6}, {-1,0,6}, {0,0,6}, {1,0,6}, {-1,-1,6}, {0,-1,6}, {1,-1,6},
					{-1,1,7}, {0,1,7}, {1,1,7}, {-1,0,7}, {0,0,7}, {1,0,7}, {-1,-1,7}, {0,-1,7}, {1,-1,7},
					{-1,1,8}, {0,1,8}, {1,1,8}, {-1,0,8}, {0,0,8}, {1,0,8}, {-1,-1,8}, {0,-1,8}, {1,-1,8},
					{-1,1,9}, {0,1,9}, {1,1,9}, {-1,0,9}, {0,0,9}, {1,0,9}, {-1,-1,9}, {0,-1,9}, {1,-1,9}, 
					{-1,1,10}, {0,1,10}, {1,1,10}, {-1,0,10}, {0,0,10}, {1,0,10}, {-1,-1,10}, {0,-1,10}, {1,-1,10}}; // top fixed

	for (int i = 0; i < 99; i++){
		voxel_model.v[i] += vec3{15,15,15}; 
		if (i < 90){
			voxel_model.rgb.push_back(vec3{1,1,0});
		} else {
			voxel_model.rgb.push_back(vec3{1,0,0});
		}
	}

	for (int i = 0; i < voxel_model.v.size(); i++)
	{
		mesh cube = mesh_primitive_cube(voxel_model.v[i],1.0f);
		mesh_drawable cube_drawable;
		cube_drawable.initialize_data_on_gpu(cube);
		cube_drawable.material.color = voxel_model.rgb[i];
		voxel_mesh.push_back(cube_drawable);
	}
	voxel_model.initialize();*/

	vox_from_png.front.png2rgba("../assets/front_T.png");
	vox_from_png.back.png2rgba("../assets/back_T.png");
	vox_from_png.side.png2rgba("../assets/side_T.png");
	vox_from_png.top.png2rgba("../assets/top.png");
	
	vox_from_png.buildVoxel3D();

	// Create bones
	Bone* torso = new Bone("torso", vec3{15,15,8}, vec3{15,15,16});
	Bone* head = new Bone("head", vec3{15,15,16}, vec3{15,15,28});
	Bone* rightArm = new Bone("rightArm", vec3{10,15,15}, vec3{1,15,15});
	Bone* leftArm = new Bone("leftArm", vec3{20,15,15}, vec3{29,15,15});
	Bone* shoulder = new Bone("shoulder", vec3{10,15,15}, vec3{20,15,15});
	Bone* rightLeg = new Bone("rightLeg", vec3{13,15,7}, vec3{13,15,0 });
	Bone* leftLeg = new Bone("leftLeg", vec3{17,15,7}, vec3{17,15,0});
	Bone* hip = new Bone("hip", vec3{13,15,7}, vec3{17,15,7});

	// assign skeleton's hierarchy
	vox_from_png.voxel.skeleton = {torso,head,rightArm,leftArm,rightLeg,leftLeg};
	hip->children = {rightLeg,leftLeg};
	shoulder->children = {rightArm,leftArm};
	torso->children = {shoulder, hip, head};
	vox_from_png.voxel.torsoBone = torso;

	// Assign their associated cubes
	std::vector<vec3> inRegion = {vec3{11,12,8}, vec3{10,11,17},vec3{1,15,15},vec3{20,15,15}, vec3{11,13,0}, vec3{16,13,0}};  
	std::vector<vec3> outRegion = {vec3{19,21,16}, vec3{20,23,28},vec3{10,16,17},vec3{29,16,17}, vec3{15,19,7}, vec3{19,19,7}}; 
	for (int i = 0; i < 6; i++){
		vox_from_png.voxel.assignMeatToBone(i, inRegion[i], outRegion[i]);
		vox_from_png.voxel.calculateLocalPositions(i);
	}

	for (int i = 0; i < vox_from_png.voxel.v.size(); i++)
	{
		Voxel vox = vox_from_png.voxel;
		
		mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
		mesh_drawable cube_drawable;
		cube_drawable.initialize_data_on_gpu(cube);
		cube_drawable.material.color = vox_from_png.voxel.rgb[i];
		voxel_mesh.push_back(cube_drawable);
	}
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Display cubes
	for(mesh_drawable cube : voxel_mesh)
		draw(cube, environment);

	// Drawing mode
	draw(sketch_drawable, environment);

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Mouse Mode(Only for sample arm)", &gui.mouse_mode);
	ImGui::Checkbox("Sketch Mode",&gui.sketch_mode);
	
	// Choose mode between select handle, fixed points or deform
	ImGui::RadioButton("Normal mode",&selectButton ,0);
	
	//ImGui::RadioButton("Rigging mode", &selectButton, 1);
	ImGui::RadioButton("Torso", &selectButton, 1);
	ImGui::RadioButton("Head", &selectButton, 2);
	ImGui::RadioButton("Right Arm", &selectButton, 3);
	ImGui::RadioButton("Left Arm", &selectButton, 4);
	ImGui::RadioButton("Right Leg", &selectButton, 5);
	ImGui::RadioButton("Left Leg", &selectButton, 6);

	// Test deform with keys
	ImGuiIO& io = ImGui::GetIO();
	if(io.KeysDown['E'])
		vox_from_png.reencode2img();
}

void scene_structure::mouse_move_event()
{
	if (gui.sketch_mode) {
		if (inputs.mouse.click.left) {
			// Add the new clicked position
			vec3 const p = unproject(camera_projection, camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
			sketch_drawable.push_back(p);
			curveEndPoint = p;
			
		} 
	}
	else if(gui.mouse_mode)
	{
		if(inputs.mouse.click.left)
		{
				vec3 mouse_pos = unproject(camera_projection,camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
				mouse_pos = vec3{0,mouse_pos.y,mouse_pos.z};
				vec3 translation = mouse_pos-initial_mouse_pos;
				voxel_model.v[10] += translation; 
				voxel_model.apply_spring_forces();
				
				for (int i = 0 ; i<voxel_mesh.size(); i++)
				{
					mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
					mesh_drawable cube_drawable;
					cube_drawable.initialize_data_on_gpu(cube);
					cube_drawable.material.color = voxel_model.rgb[i];
					voxel_mesh[i] = cube_drawable;
				}
		}
	}
	
	else {
		if (!inputs.keyboard.shift)
			camera_control.action_mouse_move(environment.camera_view);
	}	
}	
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
	if (!inputs.mouse.on_gui) {
		
		if (gui.mouse_mode)
		{
			if(inputs.mouse.click.last_action == last_mouse_cursor_action::click_left)
			// Check if the mouse clicked on a voxel
			//vec3 mouse_pos = unproject(camera_projection,camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
			/*int selected_voxel_idx = -1;
			float min_distance = 10.0f;
			for (int i = 0; i < voxel_model.v.size(); i++) {
				float distance = norm(mouse_pos - voxel_model.v[i]);
				if (distance < min_distance) {
					min_distance = distance;
					selected_voxel_idx = i;
					break;
				}
			}*/

			//if (selected_voxel_idx != -1) {
				// Store the selected voxel index
			//	deforming_voxel_idx = selected_voxel_idx;

				// Store the initial mouse position
				//initial_mouse_pos = vec3{0,mouse_pos.y,mouse_pos.z};
				initial_mouse_pos =  unproject(camera_projection,camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
			//} 
		}
		else if (gui.sketch_mode){
			
			if (inputs.mouse.click.last_action == last_mouse_cursor_action::click_left){
				sketch_drawable.clear();
				curveStartPoint = unproject(camera_projection, camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
				sketch_drawable.initialize_data_on_gpu();
				sketch_drawable.push_back(unproject(camera_projection, camera_control.camera_model.matrix_frame(), inputs.mouse.position.current));
			}
			else if(selectButton!=0 && inputs.mouse.click.last_action==last_mouse_cursor_action::release_left){
				vec3 startPos = vox_from_png.voxel.skeleton[selectButton - 1]->startJoint;
				vec3 oldEndPos = vox_from_png.voxel.skeleton[selectButton - 1]->endJoint;
				//std::cout << "old end " << oldEndPos <<std::endl;
				float boneLength = norm(oldEndPos - startPos);
				vec3 curveDir = curveEndPoint - curveStartPoint;
				vec3 newEndPos = startPos + (curveDir/norm(curveDir))* boneLength;
				vox_from_png.voxel.skeleton[selectButton - 1]->endJoint = newEndPos;//vox_from_png.voxel.skeleton[selectButton - 1].startJoint + (curveDir/norm(curveDir))*boneLength;
				vox_from_png.voxel.skeleton[selectButton - 1]->calculateGlobalTransformation(startPos,newEndPos);
				vox_from_png.voxel.updateCubesWithBone(selectButton-1);	

				for (int i : vox_from_png.voxel.skeleton[selectButton-1]->associatedCubes)
				{
					mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
					mesh_drawable cube_drawable;
					cube_drawable.initialize_data_on_gpu(cube);
					cube_drawable.material.color = vec3{1,1,0};
					cube_drawable.material.color = vox_from_png.voxel.rgb[i];
					voxel_mesh[i] = cube_drawable;
				}
				
			}
		} 
		
		
	}
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

