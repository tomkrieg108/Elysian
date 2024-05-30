#pragma once

//TODO:  all these headers needed for an app into 1 header file (elysian.h)
//TODO - there should be no references to openGL in this

#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/events/events.h"
#include "elysian/imgui/im_gui_viewport_data.h"
#include "elysian/model/mesh.h"
#include "elysian/scene/scene.h"
#include "elysian/camera/camera_controller.h"

namespace ely {

	class EditorLayer : public Layer
	{
	public:

		EditorLayer();
		~EditorLayer();

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
		bool OnViewportResize(EventViewportResize& e); 

	private:
		Window& m_window;
		OpenGLFramebuffer m_framebuffer, m_framebuffer_alt;
		ImGuiViewPortData m_viewport_data;
		
		Ref<Scene> m_scene;
	};
}