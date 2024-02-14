#pragma once

#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/events/events.h"
#include "elysian/camera/ortho_camera_controller.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_framebuffer.h"

class GammaTestLayer : public ely::Layer
{
public:

	GammaTestLayer() = delete;
	GammaTestLayer(ely::Window& window);
	~GammaTestLayer() = default;

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
	ely::OrthoCameraController m_camera_controller;

	ely::OpenGLVertexBuffer* m_vbo_grid, * m_vbo_world_coords, *m_vbo_gray_scale, *m_vbo_gray_scale_gc;
	ely::OpenGLVertexArray m_vao_grid, m_vao_world_coords, m_vao_gray_scale, m_vao_gray_scale_gc;
	ely::OpenGLTexture2D* m_texture = nullptr;
	ely::OpenGLTexture2D* m_texture_srgb = nullptr;
	ely::OpenGLFramebuffer* m_framebuffer = nullptr;
	std::unique_ptr<ely::Shader> m_gamma_test_shader;
};