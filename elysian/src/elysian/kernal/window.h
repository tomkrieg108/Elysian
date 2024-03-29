#pragma once
#include "base.h"
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
			std::string title{ "Elysiun Engine" };
			uint32_t width = 1200;
			uint32_t height = 800;
			int32_t buffer_height = 1200; //set in Init()
			int32_t buffer_width = 800; //set in Init()
			bool vsync_enabled = true;
			bool cursor_enabled = true;
			bool maximized = true;
			glm::vec4 clear_colour = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
		};
		
	public:
		
		~Window();
		Window(const WindowParams& initial_params); //Compile error if made private (due to unique_pointer<Window>

		uint32_t Height() const { return m_params.height; } ;
		uint32_t Width() const { return m_params.width; }
		uint32_t BufferHeight() const { return m_params.buffer_height; };
		uint32_t BufferWidth() const { return m_params.buffer_width; };
		float AspectRatio() const {
			return (float)BufferWidth() / (float)BufferHeight();
		}

		GLFWwindow* GetWindowHandle() const;
		Input& GetInput() { return m_input; }
		bool GetCursorEnabled() const { return m_params.cursor_enabled; }
		void SetCursorEnabled(bool enabled);
		void SetClearColour(glm::vec4 clear_colour) { m_params.clear_colour = clear_colour; }
		void ToggleCursorEnabled();
		bool IsMinimised() const;
		void Clear();
		bool ShouldClose();
		void OnUpdate();
		void ShutDown();

		static Window* Create(const WindowParams& initial_params = WindowParams());
		
	private:
		void Init(const WindowParams& initial_params);
		static void ErrorCallback(int error, const char* description);

	private:
		GLFWwindow* m_window;
		OpenGLContext* m_context = nullptr;
		WindowParams m_params;
		Input m_input;
		static uint32_t s_window_count;
	};

}
