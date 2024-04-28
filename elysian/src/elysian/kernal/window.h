#pragma once
#include "base.h"
#include "elysian/kernal/log.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_context.h" //must be included before<GLFW\glfw3.h>
#include "elysian/kernal/input.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace ely
{
	class Window
	{
	public:
		struct WindowParams
		{
			std::string title{ "App" };
			uint32_t width = 1200;
			uint32_t height = 800;
			int32_t buffer_height = 1200; //set in Init()
			int32_t buffer_width = 800; //set in Init()
			bool vsync_enabled = true;
			bool cursor_enabled = true;
			float monitor_aspect_ratio;
			glm::vec4 clear_colour = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); //TODO - should be a renderer parameter
		};
		
	public:
		
		~Window();
		Window(const WindowParams& initial_params); 

		uint32_t Height() const { return m_params.height; } 
		uint32_t Width() const { return m_params.width; }
		uint32_t BufferWidth() const { return m_params.buffer_width; }
		uint32_t BufferHeight() const { return m_params.buffer_height; }
		float AspectRatio() const { return (float)BufferWidth() / (float)BufferHeight(); }
		float MonitorAspectRatio() const {return m_params.monitor_aspect_ratio;}


		GLFWwindow* GetWindowHandle() const;
		bool GetCursorEnabled() const { return m_params.cursor_enabled; }
		void SetCursorEnabled(bool enabled);
		
		void SetClearColour(glm::vec4 clear_colour) { m_params.clear_colour = clear_colour; } //TODO -in renderer!

		bool IsMinimised() const;
		void Clear();
		void OnUpdate();
		

		void SetVSync(bool enabled);
		bool IsVSyncEnabled() const;

		static Window* Create(const WindowParams& initial_params = WindowParams());

		//Cherno - TODO - better to set this up event_handler
		void SetEventCallback(const EventCallbackFn& callback) { m_event_callback = callback; }
		EventCallbackFn& GetEventCallback() { return m_event_callback; }

	private:
		void Init(const WindowParams& initial_params);
		static void ErrorCallback(int error, const char* description);

	private:
		GLFWwindow* m_window = nullptr;
		OpenGLContext* m_context = nullptr;
		WindowParams m_params;
		static uint32_t s_window_count; //TODO - needed?

	
		/*
			Cherno
			This set by Application and runs in Application to process events. 
			Is called in Window.  
			TODO better to be in event dispatcher thing
		*/
		EventCallbackFn m_event_callback;
	};

}
