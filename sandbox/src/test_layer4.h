 #pragma once

//TODO:  all these headers needed for an app into 1 header file (elysian.h)
//TODO - there should be no references to openGL in this

#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/events/events.h"
#include "elysian/model/mesh.h"
#include "elysian/scene/scene.h"
//#include "elysian/camera/perspective_camera_controller.h"

#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/renderer/opengl_uniform_buffer.h"

#include "elysian/model/model_lgl.h"

#include "panels/scene_heirachy_panel.h"


namespace ely {

	class TestLayer4 : public ely::Layer
	{
	public:

		TestLayer4();
		~TestLayer4() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(double time_step) override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;

	private:
		bool OnKeyPressed(EventKeyPressed& e);
		bool OnMouseMoved(EventMouseMoved& e);
		bool OnMouseScrolled(EventMouseScrolled& e);
		bool OnMouseButtonPressed(EventMouseButtonPressed& e);
		bool OnWindowResize(EventWidowResize& e); //TODO - should this be handled by the application class?

	private:
		Window& m_window;
		OpenGLFramebuffer m_framebuffer, m_framebuffer_alt;
		Ref<Scene> m_scene;
		SceneHeirachyPanel m_scene_heirachy_panel;
		Entity m_main_camera_entity, m_alt_camera_entity;
	};

}