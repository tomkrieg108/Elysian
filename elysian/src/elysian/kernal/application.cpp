#include "pch.h"
#include "elysian/kernal/application.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/imgui/im_gui_layer.h"

using namespace std::literals;  // For suffixes
using namespace std::chrono_literals;

namespace ely
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& title)
	{
		//TODO: include assertion / check that s_instance doesn't already exist ?
		s_instance = this;
		Window::WindowParams window_params;
		window_params.title = title;
		m_window = Window::Create(window_params);

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
		Application::PopOverlay(m_imgui_layer);
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

	bool Application::OnWindowClose(EventWidowClose& e)
	{
		//CORE_INFO("Application::OnWindowClose() called {}", e);
		m_running = false;
		return true;
	}

	bool Application::OnWindowResize(EventWidowResize& e)
	{
		//CORE_INFO("Application::OnWindowResize() called {}", e);
		return true;
	}

	bool Application::OnWindowFocusChange(EventWindowFocusChange& e)
	{
		//CORE_INFO("Application::OnWindowFocusChange() called {}", e);
		return true;
	}

	bool Application::OnWindowIconifyChange(EventWindowIconifyChange& e)
	{
		//CORE_INFO("Application::OnWindowIconifyChange() called {}", e);
		return true;
	}

	void Application::OnEvent(Event& e)
	{
		/*
		Cherno
		This is going to get called from window.cpp via m_event_callback
		*/
		EventDispatcher dispatcher(e);

		//NOTE: & is compulsory for member functions, optional for free functions
		dispatcher.Dispatch<EventWidowClose>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1 ));
		dispatcher.Dispatch<EventWindowFocusChange>(std::bind(&Application::OnWindowFocusChange, this, std::placeholders::_1));
		dispatcher.Dispatch<EventWindowIconifyChange>(std::bind(&Application::OnWindowIconifyChange, this, std::placeholders::_1));

		//std::function<bool(Event&)> callback = std::bind(&Application::OnWindowResize, this, std::placeholders::_1); //compile error!
		//auto callback = std::bind(&Application::OnWindowResize, this, std::placeholders::_1); //ok!
		
		for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->OnEvent(e);
		}
	}


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
			{
				m_window->OnUpdate(); //without this iconified window doesn't re-opened
				continue;
			}
				
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