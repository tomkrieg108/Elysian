#pragma once

//TODO:  all these headers needed for an app into 1 header file (elysian.h)
//TODO - there should be no references to openGL in this

#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/events/events.h"
#include "elysian/model/mesh.h"
#include "elysian/scene/scene.h"
#include "elysian/camera/perspective_camera_controller.h"

#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/renderer/opengl_uniform_buffer.h"

#include "elysian/model/model_lgl.h"


class TestLayer2 : public ely::Layer
{
public:

	TestLayer2() = delete;
	TestLayer2(ely::Window& window);
	~TestLayer2() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(double time_step) override;
	void OnImGuiRender() override;

private:
	void OnKeyPressed(ely::EventKeyPressed& e);
	void OnMouseMoved(ely::EventMouseMoved& e);
	void OnMouseScrolled(ely::EventMouseScrolled& e);
	void OnMouseButtonPressed(ely::EventMouseButtonPressed& e);
	void OnWindowResize(ely::EventWidowResize& e);

private:
	ely::Window& m_window;

	ely::PerspectiveCameraController2 m_camera_controller, m_camera_controller_alt;
	ely::PerspectiveCameraController2* m_active_controller = &m_camera_controller;

	ely::OpenGLFramebuffer m_framebuffer, m_framebuffer_alt;

	ely::OpenGLUniformBuffer m_ubuff_camera;

	ely::Ref<ely::Shader> m_cube_shader, m_grid_shader, m_light_shader, m_plane_shader, m_model_shader;


	//cube parameters
	glm::mat4 m_cube_model = glm::mat4(1.0f);
	glm::vec3 m_cube_pos = glm::vec3(-2.0f, 2.0f, 3.0f);
	float m_cube_shininess = 64.0f;
	
	//light parameters
	glm::vec3 m_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 m_light_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 m_light_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 m_light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 m_model_plane = glm::mat4(1.0); // model matrix of the yellow square!

	lgl::Model* m_model = nullptr;

	ely::Ref<ely::Mesh> m_cube_mesh, m_quad_mesh, m_world_coords_mesh, m_cube_coords_mesh, m_camera_coords_mesh
		,m_camera_alt_coords_mesh, m_light_mesh, m_camera_mesh, m_camera_alt_mesh;

	ely::Ref<ely::Mesh> m_grid_mesh;
	
};


