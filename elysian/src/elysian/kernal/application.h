#pragma once
#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/imgui/im_gui_layer.h"


int main();

namespace ely
{
	class Application
	{
	public:
		//This is effectively a singleton
		//Ctr should be private (protected?).  Public static init method to call ctr
		Application(const std::string& title = "Elysian App");
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);
		virtual void OnUpdate(double delta_time) {}
		Window& GetWindow() { return *m_window; }

		ImGuiLayer& GetImGuiLayer() { return *m_imgui_layer; }
		void Close();

		static Application& GetInstance() { return *s_instance; }

		void OnEvent(Event& e);
		void Run();

	private:

		//Application level events - handled in application.cpp
		bool OnWindowClose(EventWidowClose& e);
		bool OnWindowResize(EventWidowResize& e);
		bool OnWindowFocusChange(EventWindowFocusChange& e);
		bool OnWindowIconifyChange(EventWindowIconifyChange& e);

		template<typename T, typename E>
		static auto MakeCallback(void (T::* callback_fn)(E&), T* instance) 
		{
			std::function<void(E&)> val = std::bind(callback_fn, instance, std::placeholders::_1);
			return std::bind(callback_fn, instance, std::placeholders::_1);
		}

		template<typename T, typename E>
		auto MakeCallback(void (T::* callback_fn)(E&)) { return std::bind(callback_fn, this, std::placeholders::_1);}
		
	private:
		Window* m_window = nullptr;
		ImGuiLayer* m_imgui_layer = nullptr;
		LayerStack m_layer_stack;
		bool m_running = true;

		static Application* s_instance;
	};

	Application* CreateApplication(); //defined in client
}