#pragma once

//TODO:  all these headers needed for an app into 1 header file (elysian.h)

#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/events/events.h"
#include "elysian/camera/perspective_camera_controller.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/renderer/opengl_uniform_buffer.h"

#include "elysian/model/model_lgl.h"


class TestLayer1 : public ely::Layer
{
public:

	TestLayer1() = delete;
	TestLayer1(ely::Window& window);
	~TestLayer1() = default;

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

	ely::PerspectiveCameraController m_camera_controller, m_camera_controller_alt;
	ely::PerspectiveCameraController* m_active_controller = &m_camera_controller;

	ely::OpenGLVertexBuffer* m_vbo_grid = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_cube = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_light = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_camera = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_camera_alt = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_world_coords = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_cube_coords = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_camera_coords = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_camera_alt_coords = nullptr;
	ely::OpenGLVertexBuffer* m_vbo_square_xz = nullptr;

	ely::OpenGLVertexArray m_vao_grid, m_vao_cube, m_vao_light, m_vao_camera, m_vao_camera_alt, 
		m_vao_world_coords, m_vao_cube_coords, m_vao_camera_coords, m_vao_camera_alt_coords, m_vao_square_xz;

	ely::OpenGLFramebuffer m_framebuffer, m_framebuffer_alt;

	ely::OpenGLUniformBuffer m_ubuff_camera;

	ely::Ref<ely::Shader> m_cube_shader, m_grid_shader, m_light_shader, m_plane_shader, m_model_shader;


	//cube parameters
	glm::mat4 m_cube_model = glm::mat4(1.0f);
	glm::vec3 m_cube_pos = glm::vec3(-2.0f, 2.0f, 3.0f);
	float m_cube_shininess = 64.0f;
	ely::OpenGLTexture2D* m_diffuse_map_tex = nullptr;
	ely::OpenGLTexture2D* m_specular_map_tex = nullptr;

	//light parameters
	glm::vec3 m_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 m_light_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 m_light_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 m_light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 m_model_plane = glm::mat4(1.0); // model matrix of the yellow square!

	lgl::Model* m_model = nullptr;
};


