#include "pch.h"
#include "elysian/kernal/application.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/imgui/im_gui_layer.h"

namespace ely
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& title)
	{
		//TODO: include assertion / check that s_instance doesn't already exist ?
		s_instance = this;
		Window::WindowParams params;
		params.title = true;
		m_window = Window::Create(params);

		OpenGLRenderer::Init();
		Texture2DRepo::Init();
		ShaderRepo::Init();
		MaterialRepo::Init();

		//TODO Maybe makes more sense than to set the function in event dispatcher rather than window as in hazel
		m_window->SetEventCallback(MakeCallback(&Application::OnEvent));
	
		m_imgui_layer = new ImGuiLayer();
		PushOverlay(m_imgui_layer);
	}

	Application::~Application()
	{
		delete m_imgui_layer;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layer_stack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_layer_stack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_layer_stack.PopLayer(layer); 
		//TODO - OnDetatch() ?
	}

	void Application::PopOverlay(Layer* layer)
	{
		m_layer_stack.PopOverlay(layer); //also calls OnDetatch()
	}

	void Application::Close()
	{
		m_running = false;
	}

	bool Application::OnWindowClose(events_v2::EventWidowClose& e)
	{
		CORE_INFO("Application::OnWindowClose() called {}", e);
		m_running = false;
		return true;
	}

	bool Application::OnWindowResize(events_v2::EventWidowResize& e)
	{
		CORE_INFO("Application::OnWindowResize() called {}", e);
		return true;
	}

	
#if 1
	void Application::OnEvent(events_v2::Event& e)
	{
		/*
		Cherno
		This is going to get called from window.cpp via m_event_callback
		*/

		CORE_TRACE("Application::OnEvent() {} ", e);
		events_v2::EventDispatcher dispatcher(e);

		//NOTE: & is compulsory for member functions, optional for free functions
		dispatcher.Dispatch<events_v2::EventWidowClose>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1 ));
		dispatcher.Dispatch<events_v2::EventWidowResize>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

		//std::function<bool(Event&)> callback = std::bind(&Application::OnWindowResize, this, std::placeholders::_1); //compile error!
		//auto callback = std::bind(&Application::OnWindowResize, this, std::placeholders::_1); //ok!
		

		for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->OnEvent(e);
		}
	}
#endif

	void Application::Run()
	{
		APP_INFO("Main loop Running.");
		auto delta_time = 0.0;
		auto last_time = glfwGetTime();
		while (m_running)
		{
			auto now = glfwGetTime(); //in seconds
			delta_time = now - last_time;
			last_time = now;

			if (m_window->IsMinimised())
				continue;

			m_window->Clear();

			this->OnUpdate(delta_time);

			for (Layer* layer : m_layer_stack)
				layer->OnUpdate(delta_time);

			m_imgui_layer->Begin();
				for (Layer* layer : m_layer_stack)
					layer->OnImGuiRender();
			m_imgui_layer->End();

			m_window->OnUpdate();
		}
	}
}