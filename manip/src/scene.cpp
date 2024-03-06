#include "scene.hpp"


// Test scene to check correct compilation and run
//  The code should display a rotating cube and a textured quad on top of a circular ground.

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
	camera_control.look_at({ 3.0f, 2.0f, 2.0f }, { 0,0,0 }, { 0,0,1 });

	// Create a visual frame representing the coordinate system
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

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
	//vox_from_png.voxel.initialize();

	// Create bones
	Bone torso = Bone("torso", vec3{15,15,8}, vec3{15,15,16});
	Bone head = Bone("head", vec3{16,16,17}, vec3{15,15,28});
	Bone rightArm = Bone("rightArm", vec3{10,15,15}, vec3{1,15,15});
	Bone leftArm = Bone("leftArm", vec3{20,15,15}, vec3{29,15,15});
	Bone rightLeg = Bone("rightLeg", vec3{13,15,7}, vec3{13,15,0 });
	Bone leftLeg = Bone("leftLeg", vec3{17,15,7}, vec3{17,15,0});
	vox_from_png.voxel.skeleton = {torso,head,rightArm,leftArm,rightLeg,leftLeg};

	// Assign their associated cubes
	std::vector<vec3> inRegion = {vec3{11,12,8}, vec3{10,11,17},vec3{1,15,15},vec3{20,15,15}, vec3{11,13,0}, vec3{16,13,0}};  
	std::vector<vec3> outRegion = {vec3{19,21,16}, vec3{20,23,28},vec3{10,16,17},vec3{29,16,17}, vec3{15,19,7}, vec3{19,19,7}};
	for (int i = 0; i < 6; i++)
		vox_from_png.voxel.assignMeatToBone(i, inRegion[i], outRegion[i]);
	
	for(int i = 0; i < 6; i++)
		std::cout <<"Cubes : " << vox_from_png.voxel.skeleton[i].associatedCubes.size() << std::endl;
	std::cout <<vox_from_png.voxel.v.size()<<std::endl;

	for (int i = 0; i < vox_from_png.voxel.v.size(); i++)
	{
		mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
		mesh_drawable cube_drawable;
		cube_drawable.initialize_data_on_gpu(cube);
		cube_drawable.material.color = vox_from_png.voxel.rgb[i];
		voxel_mesh.push_back(cube_drawable);
	}

	/*mesh z = mesh_primitive_cube(vec3{10,15,14},1.0f);
	mesh_drawable zd;
	zd.initialize_data_on_gpu(z);
	zd.material.color = vec3{1,1,0};
	voxel_mesh.push_back(zd);*/
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	// Display cube
	// ********************************************* //
	for(mesh_drawable cube : voxel_mesh)
		draw(cube, environment);

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	// Choose mode between select handle, fixed points or deform
	ImGui::RadioButton("Normal mode",&selectButton ,0);
	//ImGui::RadioButton("Rigging mode", &selectButton, 1);
	ImGui::RadioButton("Torso", &selectButton, 1);
	ImGui::RadioButton("Head", &selectButton, 2);
	ImGui::RadioButton("Right Arm", &selectButton, 3);
	ImGui::RadioButton("Left Arm", &selectButton, 4);
	ImGui::RadioButton("Right Leg", &selectButton, 5);
	ImGui::RadioButton("Left Leg", &selectButton, 6);

	ImGuiIO& io = ImGui::GetIO();

	if(selectButton == 0){
		isRigging = false;
	} else {
		isRigging = true;
	}
	
	//if(io.KeysDown['E'])
	//	vox_from_png.reencode2img();
	if (io.KeysDown['S'])
	{
		vec3 pivotPoint = vec3{10,15,15};
		mat4 translateToOrigin = mat4::build_translation(-pivotPoint);
		mat4 translateBack = mat4::build_translation(pivotPoint);
		mat4 rotationMat = mat4::build_rotation_from_axis_angle(vec3{0,1,0},-3.14/6);
		mat4 transformMat = translateBack * rotationMat * translateToOrigin;
		for(int i = 0; i < vox_from_png.voxel.skeleton[2].associatedCubes.size(); i++)
		{
			vec4 cubePos = vec4{vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]],1};
			cubePos =  transformMat* cubePos;
			//vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]] = vec3{cubePos.x,cubePos.y,cubePos.z};
			vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]] = vec3{round(cubePos.x),round(cubePos.y),round(cubePos.z)};
		}
		
		//vox_from_png.voxel.skeleton[2].updateGlobalTransformation(transformMat);
		//vox_from_png.voxel.updateCubesWithBone(2);s
		for (int i = 0 ; i<voxel_mesh.size(); i++)
		{
			mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
			mesh_drawable cube_drawable;
			cube_drawable.initialize_data_on_gpu(cube);
			cube_drawable.material.color = vox_from_png.voxel.rgb[i];
			voxel_mesh[i] = cube_drawable;
		} 
	}
	if (io.KeysDown['D'])
	{
		vec3 pivotPoint = vec3{10,15,15};
		mat4 translateToOrigin = mat4::build_translation(-pivotPoint);
		mat4 translateBack = mat4::build_translation(pivotPoint);
		mat4 rotationMat = mat4::build_rotation_from_axis_angle(vec3{1,0,0},-3.14/6);
		mat4 transformMat = translateBack * rotationMat * translateToOrigin;
		for(int i = 0; i < vox_from_png.voxel.skeleton[2].associatedCubes.size(); i++)
		{
			vec4 cubePos = vec4{vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]],1};
			cubePos =  transformMat* cubePos;
			//vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]] = vec3{cubePos.x,cubePos.y,cubePos.z};
			vox_from_png.voxel.v[vox_from_png.voxel.skeleton[2].associatedCubes[i]] = vec3{round(cubePos.x),round(cubePos.y),round(cubePos.z)};
		}
		
		//vox_from_png.voxel.skeleton[2].updateGlobalTransformation(transformMat);
		//vox_from_png.voxel.updateCubesWithBone(2);s
		for (int i = 0 ; i<voxel_mesh.size(); i++)
		{
			mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
			mesh_drawable cube_drawable;
			cube_drawable.initialize_data_on_gpu(cube);
			cube_drawable.material.color = vox_from_png.voxel.rgb[i];
			voxel_mesh[i] = cube_drawable;
		} 
	}
	if (io.KeysDown['X'])
	{
		vec3 pivotPoint = vec3{17,15,7};
		mat4 translateToOrigin = mat4::build_translation(-pivotPoint);
		mat4 translateBack = mat4::build_translation(pivotPoint);
		mat4 rotationMat = mat4::build_rotation_from_axis_angle(vec3{0,1,0},3.14/6);
		mat4 transformMat = translateBack * rotationMat * translateToOrigin;
		for(int i = 0; i < vox_from_png.voxel.skeleton[4].associatedCubes.size(); i++)
		{
			vec4 cubePos = vec4{vox_from_png.voxel.v[vox_from_png.voxel.skeleton[4].associatedCubes[i]],1};
			cubePos =  transformMat * cubePos;
			vox_from_png.voxel.v[vox_from_png.voxel.skeleton[4].associatedCubes[i]] = vec3{cubePos.x,cubePos.y,cubePos.z};
			//vox_from_png.voxel.v[vox_from_png.voxel.skeleton[5].associatedCubes[i]] = vec3{round(cubePos.x),round(cubePos.y),round(cubePos.z)};
		}
		
		//vox_from_png.voxel.skeleton[2].updateGlobalTransformation(transformMat);
		//vox_from_png.voxel.updateCubesWithBone(2);s
		for (int i = 0 ; i<voxel_mesh.size(); i++)
		{
			mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
			mesh_drawable cube_drawable;
			cube_drawable.initialize_data_on_gpu(cube);
			cube_drawable.material.color = vox_from_png.voxel.rgb[i];
			voxel_mesh[i] = cube_drawable;
		} 
	}
	if (io.KeysDown['C'])
	{
		vec3 pivotPoint = vec3{13,15,7};
		mat4 translateToOrigin = mat4::build_translation(-pivotPoint);
		mat4 translateBack = mat4::build_translation(pivotPoint);
		mat4 rotationMat = mat4::build_rotation_from_axis_angle(vec3{1,0,0},-3.14/6);
		mat4 transformMat = translateBack * rotationMat * translateToOrigin;
		for(int i = 0; i < vox_from_png.voxel.skeleton[4].associatedCubes.size(); i++)
		{
			vec4 cubePos = vec4{vox_from_png.voxel.v[vox_from_png.voxel.skeleton[4].associatedCubes[i]],1};
			cubePos =  transformMat* cubePos;
			vox_from_png.voxel.v[vox_from_png.voxel.skeleton[4].associatedCubes[i]] = vec3{cubePos.x,cubePos.y,cubePos.z};
			//vox_from_png.voxel.v[vox_from_png.voxel.skeleton[4].associatedCubes[i]] = vec3{round(cubePos.x),round(cubePos.y),round(cubePos.z)};
		}
		
		//vox_from_png.voxel.skeleton[2].updateGlobalTransformation(transformMat);
		//vox_from_png.voxel.updateCubesWithBone(2);s
		for (int i = 0 ; i<voxel_mesh.size(); i++)
		{
			mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
			mesh_drawable cube_drawable;
			cube_drawable.initialize_data_on_gpu(cube);
			cube_drawable.material.color = vox_from_png.voxel.rgb[i];
			voxel_mesh[i] = cube_drawable;
		} 
	}
}

void scene_structure::mouse_move_event()
{
	if(isRigging)
	{
		if(inputs.mouse.click.left)
		{
			//if(deforming_voxel_idx != -1)
			//{
				//vec3 mouse_pos = unproject(camera_projection,camera_control.camera_model.matrix_frame(), inputs.mouse.position.current);
				//mouse_pos = vec3{0,mouse_pos.y,mouse_pos.z};
				//vec3 translation = mouse_pos-initial_mouse_pos;
				//translation = translation/norm(translation);
				vec2 mouse_pos = inputs.mouse.position.current; 
				vec2 translation = (mouse_pos - initial_mouse_pos);
				
				mat4 translationMatrix = mat4::build_translation(vec3{translation.x,translation.y,0.0f});
				
				float rotationAngle = atan2(translation.y, translation.x);
				float scalingFactor = norm(translation);
				mat4 rotationMatrix = mat4::build_rotation_from_axis_angle(vec3{0,0,1},rotationAngle);
				mat4 scalingMatrix = mat4::build_scaling(scalingFactor,scalingFactor,1);

				mat4 transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;

				vox_from_png.voxel.skeleton[selectButton-1].updateGlobalTransformation(transformationMatrix);
				vox_from_png.voxel.updateCubesWithBone(selectButton-1);
				//vec3 mouse_moved_int{std::round(mouse_moved.x), std::round(mouse_moved.y*2),std::round(mouse_moved.z)};

				//if(abs(mouse_moved.x) >= 1.0f || abs(mouse_moved.y) >= 1.0f || abs(mouse_moved.z) >= 1.0f)
				//{
					//vox_from_png.voxel.v[10] += translation; //mouse_moved_int;//vec3{std::round(mouse_moved.x), std::round(mouse_moved.y),std::round(mouse_moved.z)};
					//vox_from_png.voxel.apply_spring_forces();
				
					for (int i = 0 ; i<voxel_mesh.size(); i++)
					{
						mesh cube = mesh_primitive_cube(vox_from_png.voxel.v[i],1.0f);
						mesh_drawable cube_drawable;
						cube_drawable.initialize_data_on_gpu(cube);
						cube_drawable.material.color = vox_from_png.voxel.rgb[i];
						voxel_mesh[i] = cube_drawable;
					}
					//initial_mouse_pos = mouse_pos;
				//}
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
		
		if (isRigging && inputs.mouse.click.last_action == last_mouse_cursor_action::click_left)
		{
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
				initial_mouse_pos =  inputs.mouse.position.current;//.x, inputs.mouse.position.current.y};
			//} 
		}
		
	}
}
void scene_structure::keyboard_event()
{
	//ImGuiIO& io = ImGui::GetIO();
	camera_control.action_keyboard(environment.camera_view);
	/*if (isRigging){
		if (selectButton == 2 && io.KeysDown['S'])
			{
				vec3 pivotPoint = vec3{10,15,15};
				mat4 translateToOrigin = mat4::build_translation(-pivotPoint);
				mat4 translateBack = mat4::build_translation(pivotPoint);
				mat4 rotationMat = mat4::build_rotation_from_axis_angle(vec3{0,1,0},3.14/6);
				mat4 transformMat = translateBack * rotationMat*translateToOrigin;
				std::cout<< "Transformation : " << transformMat<<std::endl;
				vox_from_png.voxel.skeleton[2].updateGlobalTransformation(transformMat);
				vox_from_png.voxel.updateCubesWithBone(2);
			}
	}*/
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

