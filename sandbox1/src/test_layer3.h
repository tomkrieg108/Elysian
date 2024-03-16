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


class TestLayer3 : public ely::Layer
{
public:

	TestLayer3() = delete;
	TestLayer3(ely::Window& window);
	~TestLayer3() = default;

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
	ely::OpenGLFramebuffer m_framebuffer, m_framebuffer_alt;
	ely::Scene m_scene;
};


