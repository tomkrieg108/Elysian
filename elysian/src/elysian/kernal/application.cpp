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
	}

	void Application::PopOverlay(Layer* layer)
	{
		m_layer_stack.PopOverlay(layer); //also calls OnDetatch()
	}

	void Application::Run()
	{
		APP_INFO("Main loop Running.");
		auto delta_time = 0.0;
		auto last_time = glfwGetTime();
		while (!m_window->ShouldClose())
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
		Close();
	}

	void Application::Close()
	{
		m_layer_stack.PopOverlay(m_imgui_layer);
	}
}