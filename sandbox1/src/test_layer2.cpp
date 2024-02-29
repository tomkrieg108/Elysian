#include "pch.h"

#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/model/mesh.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"

#include "test_layer2.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

#include <cmath>

//TODO:  want to set this up so that there are no references to any openGL calls

TestLayer2::TestLayer2(ely::Window& window) :
	m_window{ window },
	m_camera_controller(window.AspectRatio()),
	m_camera_controller_alt(window.AspectRatio(), glm::vec3(4.0, 1.0, 7.0)),
	m_framebuffer(window.BufferWidth(), window.BufferHeight()),
	m_framebuffer_alt(window.BufferWidth(), window.BufferHeight())
{

	//TODO - this event stuff shouldn't be in the app
	ely::EventDispatcher::SetCallback(this, &TestLayer2::OnKeyPressed);
	ely::EventDispatcher::SetCallback(this, &TestLayer2::OnMouseMoved);
	ely::EventDispatcher::SetCallback(this, &TestLayer2::OnMouseScrolled);
	ely::EventDispatcher::SetCallback(this, &TestLayer2::OnWindowResize);
	ely::EventDispatcher::SetCallback(this, &TestLayer2::OnMouseButtonPressed);


	m_scene.Init();

	ely::OpenGLRenderer::SetLineWidth(1.0);

	// Initialise meshes
	m_light_mesh = ely::MeshPrimitive::GetCubeMesh();
	m_light_mesh->SetMaterial(ely::MaterialRepo::Get("empty"));
	m_camera_mesh = ely::MeshPrimitive::GetCubeMesh();
	m_camera_mesh->SetMaterial(ely::MaterialRepo::Get("empty"));
	m_camera_alt_mesh = ely::MeshPrimitive::GetCubeMesh();
	m_camera_alt_mesh->SetMaterial(ely::MaterialRepo::Get("empty"));

	m_cube_mesh = ely::MeshPrimitive::GetCubeMesh();
	m_quad_mesh = ely::MeshPrimitive::GetQuadMesh();
	m_grid_mesh = ely::MeshPrimitive::GetGridMesh(20.0f, 1.0f);

	m_world_coords_mesh = ely::MeshPrimitive::GetCoordSystemMesh(glm::mat4(1), 20.0f);
	m_cube_coords_mesh = ely::MeshPrimitive::GetCoordSystemMesh(glm::mat4(1), 2.0f);
	m_camera_coords_mesh = ely::MeshPrimitive::GetCoordSystemMesh(glm::mat4(1), 2.0f);
	m_camera_alt_coords_mesh = ely::MeshPrimitive::GetCoordSystemMesh(glm::mat4(1), 2.0f);
	

	//buffer setup
	//m_vbo_grid = ely::MeshPrimitive::GetGridVertexBuffer(20.0f, 1.0f);
	//m_vbo_cube = ely::MeshPrimitive::GetCubeVertexBuffer();
	//m_vbo_light = ely::MeshPrimitive::GetCubeVertexBuffer();
	//m_vbo_camera = ely::MeshPrimitive::GetCubeVertexBuffer();
	//m_vbo_camera_alt = ely::MeshPrimitive::GetCubeVertexBuffer();
	//m_vbo_square_xz = ely::MeshPrimitive::GetSquareXZVertexBuffer();

	//m_vbo_world_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 20.0f);
	//m_vbo_cube_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 2.0f);
	//m_vbo_camera_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 2.0f);
	//m_vbo_camera_alt_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 2.0f);

	//m_vao_grid.AddVertexBuffer(*m_vbo_grid);
	//m_vao_cube.AddVertexBuffer(*m_vbo_cube);
	//m_vao_light.AddVertexBuffer(*m_vbo_light);
	//m_vao_camera.AddVertexBuffer(*m_vbo_camera);
	//m_vao_camera_alt.AddVertexBuffer(*m_vbo_camera_alt);
	//m_vao_square_xz.AddVertexBuffer(*m_vbo_square_xz);

	//m_vao_world_coords.AddVertexBuffer(*m_vbo_world_coords);
	//m_vao_cube_coords.AddVertexBuffer(*m_vbo_cube_coords);
	//m_vao_camera_coords.AddVertexBuffer(*m_vbo_camera_coords);
	//m_vao_camera_alt_coords.AddVertexBuffer(*m_vbo_camera_alt_coords);

	//shader setup
	m_cube_shader = ely::ShaderRepo::Get("light_map_diff_spec");
	m_light_shader = ely::ShaderRepo::Get("white");
	m_grid_shader = ely::ShaderRepo::Get("coord_sys");
	m_plane_shader = ely::ShaderRepo::Get("colored_basic");
	m_model_shader = ely::ShaderRepo::Get("model_loading");

	//TODO investigate glGetActiveUniformBlockiv()
	//TODO these gl calls should be part of the OpenGLUniformBuffer class
	//TODO according to learnopengl (advanced glsl) the steps below have to be repeated for each shader.  But seems to work without doing this!
	//TODO: can specify binding explicitly in shader: layout(std140, binding = 2) uniform Lights { ... }; which means the following isn't needed

	uint32_t binding = 0;
	uint32_t block_index = glGetUniformBlockIndex(m_cube_shader->GetProgramID(), "u_buff_camera");
	glUniformBlockBinding(m_grid_shader->GetProgramID(), binding, block_index);  //this assigns the binding (not assigned by the shader unless done explicitly as above, in which case this call isn't needed

	//block_index = 0 for all these!
	block_index = glGetUniformBlockIndex(m_light_shader->GetProgramID(), "u_buff_camera");
	glUniformBlockBinding(m_grid_shader->GetProgramID(), binding, block_index);

	block_index = glGetUniformBlockIndex(m_grid_shader->GetProgramID(), "u_buff_camera");
	glUniformBlockBinding(m_grid_shader->GetProgramID(), binding, block_index);

	block_index = glGetUniformBlockIndex(m_plane_shader->GetProgramID(), "u_buff_camera");
	glUniformBlockBinding(m_grid_shader->GetProgramID(), binding, block_index);


	/*APP_WARN("############################################################");
	APP_TRACE("u_buff_camera index: {}", block_index);
	APP_TRACE("u_buff_camera binding: {}", binding);
	APP_WARN("############################################################");*/
	m_ubuff_camera.Init(sizeof(glm::mat4) * 2, binding);

	//texture setup
	//m_diffuse_map_tex = new ely::OpenGLTexture2D("container2.png");
	//m_specular_map_tex = new ely::OpenGLTexture2D("container2_specular.png");
	//m_diffuse_map_tex = ely::Texture2DRepo::Get("container2.png").get();
	//m_specular_map_tex = ely::Texture2DRepo::Get("container2_specular.png").get();

	// shader configuration
	// --------------------

	
	m_cube_mesh->GetMaterial().UploadDataToShader(ely::ShaderRepo::Get("light_map_diff_spec"));

	//set initial position, scale, 
	m_cube_model = glm::mat4(1.0);
	m_cube_model = glm::translate(m_cube_model, m_cube_pos);

	m_plane_shader->Bind();
	//m_plane_shader->SetUniform3f("objectColor", 1.0f, 1.0f, 0.0f);
	m_plane_shader->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	m_model_plane = glm::translate(m_model_plane, glm::vec3(2.0f, 0.0, 4.0f));
	m_plane_shader->SetUniformMat4f("u_model", m_model_plane);

	//load model
	//m_model = new lgl::Model{ "../../assets/models/nanosuit/nanosuit.obj" };

	m_cube_shader->OutputInfo();
	m_light_shader->OutputInfo();
	m_grid_shader->OutputInfo();
	m_plane_shader->OutputInfo();
	m_model_shader->OutputInfo();
}


void TestLayer2::OnAttach()
{
}

void TestLayer2::OnDetach()
{
}

void TestLayer2::OnUpdate(double time_step)
{
	m_active_controller->OnUpdate(time_step);

	//-----------------------------------------------------------------------------------
	//Render to screen (main camera)
	//-----------------------------------------------------------------------------------
	ely::OpenGLRenderer::SetLineWidth(1.0);

	//grid
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));

	// Setup uniform buffer data  for main camera (view, proj)
	glm::mat4 cam_data[2];
	cam_data[0] = m_camera_controller.GetCamera().GetProjMatrix();
	cam_data[1] = m_camera_controller.GetCamera().GetViewMatrix();
	uint32_t cam_data_size = sizeof(cam_data);

	m_ubuff_camera.SetData((void*)(cam_data), cam_data_size);

	//m_vao_grid.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_grid->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_grid_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//global axes (uses same shader as grid)
	//m_vao_world_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_world_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_world_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//cube
	m_cube_shader->Bind();
	//rotate the box on local axis
	glm::vec3 rot_axis = glm::vec3(1, 1, 1);
	float angle = (float)time_step; // 1/60 radians 
	m_cube_model = glm::rotate(m_cube_model, angle, rot_axis);

	//orbit box about world y axis
	glm::mat4 orbit_axis = glm::mat4(1.0f);
	orbit_axis = glm::rotate(orbit_axis, angle, glm::vec3(0, 1, 0));
	m_cube_model = orbit_axis * m_cube_model;

	m_cube_shader->SetUniform3f("light.position", m_light_pos);
	m_cube_shader->SetUniform3f("viewPos", m_camera_controller.GetCamera().GetPosition());

	//light properties
	m_cube_shader->SetUniform3f("light.ambient", m_light_ambient);
	m_cube_shader->SetUniform3f("light.diffuse", m_light_diffuse);
	m_cube_shader->SetUniform3f("light.specular", m_light_specular);

	//material properties
	//m_cube_shader->SetUniform1f("material.shininess", m_cube_shininess);

	//transformations
	m_cube_shader->SetUniformMat4f("u_model", m_cube_model);

	// draw the cube
	ely::OpenGLRenderer::DrawMesh(m_cube_mesh, ely::ShaderRepo::Get("light_map_diff_spec"));
	

	//Draw the light
	m_light_shader->Bind();
	glm::mat4 light_model = glm::mat4(1.0);
	light_model = glm::mat4(1.0f);
	light_model = glm::translate(light_model, m_light_pos);
	light_model = glm::scale(light_model, glm::vec3(0.2f)); // a smaller cube
	m_light_shader->SetUniformMat4f("u_model", light_model);
	//m_vao_light.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_light->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_light_mesh, ely::ShaderRepo::Get("white"));

	//cube coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", m_cube_model);
	//m_vao_cube_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_cube_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_cube_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);


	//draw alt camera
	glm::mat4 cam_alt_model = m_camera_controller_alt.GetCamera().GetModelMatrix();
	m_light_shader->Bind();
	m_light_shader->SetUniformMat4f("u_model", cam_alt_model);
	//m_vao_camera_alt.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_camera_alt->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_alt_mesh, ely::ShaderRepo::Get("white"));

	//alt camera coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", cam_alt_model);
	//m_vao_camera_alt_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_camera_alt_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_alt_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//draw the plane
	m_plane_shader->Bind();
	m_plane_shader->SetUniformMat4f("u_model", m_model_plane);
	//m_vao_square_xz.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_square_xz->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_quad_mesh, ely::ShaderRepo::Get("colored_basic"));

	//draw the loaded model
	//m_model_shader->Bind();
	//m_model_shader->SetUniformMat4f("projection", m_camera_controller.GetCamera().GetProjMatrix());
	//m_model_shader->SetUniformMat4f("view", m_camera_controller.GetCamera().GetViewMatrix());
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	//m_model_shader->SetUniformMat4f("model", model);
	//m_model->Draw(*m_model_shader);

	//-----------------------------------------------------------------------------------
	//Render to framebuffer (main camera)
	//-----------------------------------------------------------------------------------

	m_framebuffer.Bind();
	ely::OpenGLRenderer::SetLineWidth(2.0);
	ely::OpenGLRenderer::SetClearColor(glm::vec4(0.1f, 0.1f, 0.4f, 1.0f));
	ely::OpenGLRenderer::ClearBuffers();

	//grid
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));

	//m_vao_grid.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_grid->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_grid_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//global axes (uses same shader as grid)
	//m_vao_world_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_world_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_world_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//cube
	ely::OpenGLRenderer::DrawMesh(m_cube_mesh, ely::ShaderRepo::Get("light_map_diff_spec"));

	//cube coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", m_cube_model);
	//m_vao_cube_coords.Bind();
	//(GL_LINES, 0, m_vbo_cube_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_cube_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//Draw the light
	m_light_shader->Bind();
	m_light_shader->SetUniformMat4f("u_model", light_model);
	//m_vao_light.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_light->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_light_mesh, ely::ShaderRepo::Get("white"));
	

	//draw alt camera
	m_light_shader->Bind();
	m_light_shader->SetUniformMat4f("u_model", cam_alt_model);
	//m_vao_camera_alt.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_camera_alt->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_alt_mesh, ely::ShaderRepo::Get("white"));

	//alt camera coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", cam_alt_model);
	//m_vao_camera_alt_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_camera_alt_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_alt_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//draw the plane
	m_plane_shader->Bind();
	m_plane_shader->SetUniformMat4f("u_model", m_model_plane);
	//m_vao_square_xz.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_square_xz->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_quad_mesh, ely::ShaderRepo::Get("colored_basic"));

	m_framebuffer.Unbind();

	//------------------------------------------------------------------------------------------
	//render to framebuffer alt
	//------------------------------------------------------------------------------------------
	m_framebuffer_alt.Bind();
	ely::OpenGLRenderer::SetLineWidth(2.0);
	ely::OpenGLRenderer::SetClearColor(glm::vec4(0.1f, 0.1f, 0.4f, 1.0f));
	ely::OpenGLRenderer::ClearBuffers();


	//grid
	m_grid_shader->Bind();

	// Setup uniform buffer data for alt camera (view, proj)
	glm::mat4 cam_alt_data[2];
	cam_alt_data[0] = m_camera_controller_alt.GetCamera().GetProjMatrix();
	cam_alt_data[1] = m_camera_controller_alt.GetCamera().GetViewMatrix();
	m_ubuff_camera.SetData((void*)(cam_alt_data), cam_data_size);

	m_grid_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));

	//m_vao_grid.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_grid->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_grid_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//global axes (uses same shader as grid)
	//m_vao_world_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_world_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_world_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//cube/
	ely::OpenGLRenderer::DrawMesh(m_cube_mesh, ely::ShaderRepo::Get("light_map_diff_spec"));

	//cube coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", m_cube_model);
	//m_vao_cube_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_cube_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_cube_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//Draw the light
	m_light_shader->Bind();
	m_light_shader->SetUniformMat4f("u_model", light_model);
	//m_vao_light.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_light->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_light_mesh, ely::ShaderRepo::Get("white"));
	//ely::OpenGLRenderer::DrawMesh(m_light_mesh);

	//draw camera
	glm::mat4 cam_model = m_camera_controller.GetCamera().GetModelMatrix();
	m_light_shader->Bind();
	m_light_shader->SetUniformMat4f("u_model", cam_model);
	//m_vao_camera.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_camera->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_mesh, ely::ShaderRepo::Get("white"));

	//camera coords
	m_grid_shader->Bind();
	m_grid_shader->SetUniformMat4f("u_model", cam_model);
	//m_vao_camera_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_camera_coords->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_camera_coords_mesh, ely::ShaderRepo::Get("coord_sys"), ely::DrawMode::Lines);

	//draw the plane
	m_plane_shader->Bind();
	m_plane_shader->SetUniformMat4f("u_model", m_model_plane);
	//m_vao_square_xz.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, m_vbo_square_xz->GetVertexCount());
	ely::OpenGLRenderer::DrawMesh(m_quad_mesh, ely::ShaderRepo::Get("colored_basic"));

	m_framebuffer_alt.Unbind();
}

void TestLayer2::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Width, Height %d %d : ", m_window.Width(), m_window.Height());
		ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window.BufferWidth(), m_window.BufferHeight());
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("Light Properties"))
	{
		ImGui::SliderFloat3("Light Pos ", &(m_light_pos[0]), -5.0f, 5.0f);
		ImGui::SliderFloat3("Ambient ", &(m_light_ambient[0]), 0.0f, 1.0f);
		ImGui::SliderFloat3("Diffuse ", &(m_light_diffuse[0]), 0.0f, 1.0f);
		ImGui::SliderFloat3("Specular ", &(m_light_specular[0]), 0.0f, 1.0f);
	}
	if (ImGui::CollapsingHeader("Box Properties"))
	{
		//ImGui::SliderFloat3("Box Pos ", &(m_cube_pos[0]), -5.0f, 5.0f);
		ImGui::SliderFloat("Shininess ", &(m_cube_shininess), 1.0f, 256.0f);
	}
	if (ImGui::CollapsingHeader("Framebuffer - main camera"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t color_attachment_id = (uint64_t)m_framebuffer.GetColourAttachmentID(); //uint64_t to stop compiler warning
		ImTextureID tex_id = (void*)color_attachment_id;
		ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Framebuffer - alt camera"))
	{
		float tex_height = 400.0;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t color_attachment_id = (uint64_t)m_framebuffer_alt.GetColourAttachmentID();
		ImTextureID tex_id = (void*)color_attachment_id;
		ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
}


void TestLayer2::OnKeyPressed(ely::EventKeyPressed& e)
{
	if (e.key == GLFW_KEY_ESCAPE)
		m_window.ShutDown();
	else if (e.key == GLFW_KEY_SPACE)
		m_window.ToggleCursorEnabled();

	else if (e.key == GLFW_KEY_T)
	{
		if (m_active_controller == std::addressof(m_camera_controller))
			m_active_controller = std::addressof(m_camera_controller_alt);
		else
			m_active_controller = std::addressof(m_camera_controller);
	}
}

void TestLayer2::OnMouseMoved(ely::EventMouseMoved& e)
{
	//APP_TRACE("(x,y) (dx,dy) {} {} {} {}", e.x, e.y, e.delta_x, e.delta_y);
	if (!m_window.GetCursorEnabled())
		m_active_controller->OnMouseMoved(e);
}

void TestLayer2::OnMouseScrolled(ely::EventMouseScrolled& e)
{
	m_active_controller->OnMouseScrolled(e);
}

void TestLayer2::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
{
	float x = (2.0f * float(e.x)) / m_window.BufferWidth() - 1.0f;
	float y = 1.0f - (2.0f * float(e.y)) / m_window.BufferHeight();
	float z = 1.0f;

	const glm::mat4& cam_model = m_camera_controller.GetCamera().GetModelMatrix();

	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(x, y, -1, 1);
	glm::vec4 ray_eye = m_camera_controller.GetCamera().GetInverseProjMatrix() * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
	glm::vec4 ray_world_4d = m_camera_controller.GetCamera().GetModelMatrix() * ray_eye; //model mat is the inverse of the view mat
	glm::vec3 ray_world = glm::normalize(glm::vec3(ray_world_4d[0], ray_world_4d[1], ray_world_4d[2]));

	glm::vec3 eye_world = glm::vec3(cam_model[3][0], cam_model[3][1], cam_model[3][2]);
	glm::vec3 grid_normal = glm::vec3(0, 1, 0);
	float t = -glm::dot(grid_normal, eye_world) / glm::dot(grid_normal, ray_world);
	glm::vec3 intersection = eye_world + t * ray_world;

	m_model_plane[3][0] = floorf(intersection.x);
	m_model_plane[3][1] = 0.0f;
	m_model_plane[3][2] = floorf(intersection.z);


	/*if (e.btn == GLFW_MOUSE_BUTTON_LEFT)
	{
		APP_TRACE("Left Mouse Btn Pressed: (x,y) {} {}", e.x, e.y);
		APP_TRACE("Ray NDC: ({}, {}, {}) ", x, y, z);
		APP_TRACE("Ray clip: ({}, {}, {} {}) ", ray_clip.x, ray_clip.y, ray_clip.z, ray_clip.w);
		APP_TRACE("Ray eye: ({}, {}, {} {}) ", ray_eye.x, ray_eye.y, ray_eye.z, ray_eye.w);
		APP_TRACE("Ray world: ({}, {}, {}) ", ray_world.x, ray_world.y, ray_world.z);
		APP_TRACE("t: ({}) ", t);
		APP_TRACE("Intersection: ({}, {}, {}) ", intersection.x, intersection.y, intersection.z);
	}

	if (e.btn == GLFW_MOUSE_BUTTON_RIGHT)
		APP_TRACE("Right Mouse Btn Pressed: (x,y) {} {}", e.x, e.y);

	if (e.btn == GLFW_MOUSE_BUTTON_MIDDLE)
		APP_TRACE("Middle Mouse Btn Pressed: (x,y) {} {}", e.x, e.y);*/
}

void TestLayer2::OnWindowResize(ely::EventWidowResize& e)
{
	//APP_TRACE("Resize: (x,y) {} {}", (float)e.buffer_width, (float)e.buffer_height);
	m_active_controller->OnWindowResize(e);
}

