#pragma once
#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/window.h"
#include "elysian/kernal/layer.h"
#include "elysian/imgui/im_gui_layer.h"

//declare entry point function here
//int main(int argc, char** argv);
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

		//can these be protected?
		void Run(); 
		void Close();

		static Application& GetInstance() { return *s_instance; }

	private:
		Window* m_window = nullptr;
		ImGuiLayer* m_imgui_layer = nullptr;
		LayerStack m_layer_stack;

		static Application* s_instance;
	};

	Application* CreateApplication(); //defined in client

}