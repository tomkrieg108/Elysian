#include "pch.h"
#include "elysian/kernal/base.h"
#include <GL\glew.h> //has to be included before glfg3.h
#include "elysian/kernal/window.h"
#include "elysian/kernal/log.h"
#include "elysian/events/events.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/kernal/input.h"

namespace ely
{
	Window::Window(const WindowParams& params) :
		m_params{ params }
	{
		Init(params);
	}
	
	Window::~Window()
	{
		glfwDestroyWindow(GetWindowHandle());
		glfwTerminate();
	}

	GLFWwindow* Window::GetWindowHandle() const
	{
		//ASSERT(m_context);
		//ASSERT(m_context->m_glfe_window);

		return m_context->GetWindowHandle();
	}

	void Window::Init(const WindowParams& params)
	{
		if (!glfwInit())
		{
			CORE_ERROR("GLFW initalisation failed");
			glfwSetErrorCallback(ErrorCallback); 
			glfwTerminate();
		} 
		CORE_INFO("GLFW Initialised");
		//NOTE: looks like the first 4 are applied by default first 4 hits a
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
		//glfwWindowHint(GLFW_SAMPLES, 4);	//for MSAA multi-sample anti-aliasing
		//See anton cha2
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
		m_params.width = video_mode->width;
		m_params.height = video_mode->height;

		auto glfw_win = glfwCreateWindow(params.width, params.height, params.title.c_str(), NULL, NULL);
		if (!glfw_win)
		{
			CORE_ERROR("GLFW window creation failed");
			glfwTerminate();
		}

		m_context = new OpenGLContext(glfw_win);
		m_context->Init();
		
		glfwGetFramebufferSize(glfw_win, &m_params.buffer_width, &m_params.buffer_height);
		if(params.vsync_enabled)
			glfwSwapInterval(1); // Enable vsync

		//set callbacks
		glfwSetMouseButtonCallback(glfw_win, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseButtonPressed(button, action, mods);
		});

		glfwSetScrollCallback(glfw_win, [](GLFWwindow* window, double xoffset, double yoffset) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseScrolled(xoffset, yoffset);
		});

		glfwSetCursorPosCallback(glfw_win, [](GLFWwindow* window, double xpos, double ypos) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseMoved(xpos, ypos);
		});


		glfwSetKeyCallback(glfw_win, [](GLFWwindow* glfw_window, int key, int code, int action, int mode) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
			win->GetInput().KeyAction(key, code, action, mode);
		});

		glfwSetWindowSizeCallback(glfw_win, [](GLFWwindow* window, int width, int height) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->m_params.width = width;
			win->m_params.height = height;
			glfwGetFramebufferSize(win->GetWindowHandle(), &(win->m_params.buffer_width), &(win->m_params.buffer_height));
			glViewport(0, 0, win->m_params.buffer_width, win->m_params.buffer_height);
			ely::EventWidowResize e{ (uint32_t)win->m_params.buffer_width, (uint32_t)win->m_params.buffer_height };
			ely::EventDispatcher::Dispatch(e);
		});

		glfwSetFramebufferSizeCallback(glfw_win, [](GLFWwindow* window, int width, int height) {
			//CORE_INFO("Frame buffer resized");
		});

		glfwSetWindowCloseCallback(glfw_win, [](GLFWwindow* window) {
				//CORE_TRACE("Window closed");
		});

		
		glfwSetCursorEnterCallback(glfw_win, [](GLFWwindow* window, int entered) {
				//CORE_INFO("Cursor enter / exit");
		});

		glfwSetCharCallback(glfw_win, [](GLFWwindow* window, unsigned int codepoint) {
			//CORE_INFO("Char typed");
		});

		glfwSetWindowPosCallback(glfw_win, [](GLFWwindow* window, int xpos, int ypos) {
			//CORE_INFO("Window moved");
		});


		glfwSetWindowFocusCallback(glfw_win, [](GLFWwindow* window, int focused) {
			//CORE_INFO("Window focus");
		});

		glfwSetWindowIconifyCallback(glfw_win, [](GLFWwindow* window, int iconified) {
			//CORE_INFO("Window iconified");
		});

		glfwSetWindowMaximizeCallback(glfw_win, [](GLFWwindow* window, int maximized) {
			//CORE_INFO("Window maximized");
		});
		
		glewExperimental = GL_TRUE;

	  glViewport(0, 0, m_params.buffer_width, m_params.buffer_height);
		glfwSetWindowUserPointer(glfw_win, this);  //Used for setup of event handlers
		SetCursorEnabled(params.cursor_enabled);
		CORE_INFO("WINDOW CREATED:");
		CORE_TRACE("   Width: {}, Height: {}, Buff Width: {}, Buff Height: {}", m_params.width, m_params.height, 
			m_params.buffer_width, m_params.buffer_height);
	}

	bool Window::IsMinimised() const
	{
		return (bool)glfwGetWindowAttrib(m_context->GetWindowHandle(), GLFW_ICONIFIED);
	}

	void Window::ToggleCursorEnabled()
	{
		if (m_params.cursor_enabled)
			glfwSetInputMode(m_context->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(m_context->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_params.cursor_enabled = !m_params.cursor_enabled;
	}

	void Window::SetCursorEnabled(bool enabled)
	{
		if(enabled)
			glfwSetInputMode(m_context->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(m_context->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_params.cursor_enabled = enabled;
	}

	std::unique_ptr<Window> Window::Create(const WindowParams& initial_params)
	{
		auto window = std::make_unique<Window>(initial_params);
		return window;
	}

	void Window::ShutDown()
	{ 
			glfwSetWindowShouldClose(m_context->GetWindowHandle(), GL_TRUE);
	}

	void Window::ClearScreeen()
	{
		//TODO:  should be in opengl_context (gl calls)
		const glm::vec4& col = m_params.clear_colour;
		glClearColor(col.r, col.g, col.b, col.a); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Window::OnUpdate()
	{
		glfwSwapBuffers(m_context->GetWindowHandle()); //TODO:  should be in opengl_context
		glfwPollEvents();
	}

	bool Window::ShouldClose()
	{
		return (bool)glfwWindowShouldClose(m_context->GetWindowHandle());
	}

	void Window::ErrorCallback(int error, const char* description)
	{
		CORE_ERROR("GLFW Error: {}", description);
	}
	
}