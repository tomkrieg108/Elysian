#pragma once
#include <GL\glew.h> //has to be included first before <glfw3.h>

struct GLFWwindow;

namespace ely
{

	class OpenGLContext
	{
	public:
		OpenGLContext(GLFWwindow* window_handle);
		~OpenGLContext() = default;

		void Init();
		void SwapBuffers();
		GLFWwindow* GetWindowHandle() const {
			return m_window_handle;
		}

	private:
		GLFWwindow* m_window_handle = nullptr;

	private:
		void DisplayGLParams();
		void DisplayVideoModes();
		
	};

}