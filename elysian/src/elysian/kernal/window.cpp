#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/window.h"
#include "elysian/kernal/log.h"
#include "elysian/events/events.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/kernal/input.h"
#include "elysian/renderer/opengl_renderer.h"

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
		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit())
		{
			CORE_ERROR("GLFW initalisation failed");
			glfwTerminate();
		} 
		CORE_INFO("GLFW Initialised");
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

		auto m_window = glfwCreateWindow(params.width, params.height, params.title.c_str(), NULL, NULL);
		if (!m_window)
		{
			CORE_ERROR("GLFW window creation failed");
			glfwTerminate();
		}

		m_context = new OpenGLContext(m_window);
		m_context->Init();
		
		glfwGetFramebufferSize(m_window, &m_params.buffer_width, &m_params.buffer_height);
		if(params.vsync_enabled)
			glfwSwapInterval(1); // Enable vsync

		//set callbacks
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseButtonPressed(button, action, mods);
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseScrolled(xoffset, yoffset);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->GetInput().MouseMoved(xpos, ypos);
		});


		glfwSetKeyCallback(m_window, [](GLFWwindow* glfw_window, int key, int code, int action, int mode) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
			win->GetInput().KeyAction(key, code, action, mode);
		});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->m_params.width = width;
			win->m_params.height = height;
			glfwGetFramebufferSize(win->GetWindowHandle(), &(win->m_params.buffer_width), &(win->m_params.buffer_height));

			//TODO:  should be forwarded to renderer
			OpenGLRenderer::SetViewport(win->m_params.buffer_width, win->m_params.buffer_height);
			//glViewport(0, 0, win->m_params.buffer_width, win->m_params.buffer_height);

			ely::EventWidowResize e{ (uint32_t)win->m_params.buffer_width, (uint32_t)win->m_params.buffer_height };
			ely::EventDispatcher::Dispatch(e);
		});

		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			//CORE_INFO("Frame buffer resized");
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
				//CORE_TRACE("Window closed");
		});

		
		glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, int entered) {
				//CORE_INFO("Cursor enter / exit");
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
			//CORE_INFO("Char typed");
		});

		glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos) {
			//CORE_INFO("Window moved");
		});


		glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
			//CORE_INFO("Window focus");
		});

		glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
			//CORE_INFO("Window iconified");
		});

		glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximized) {
			//CORE_INFO("Window maximized");
		});
		
	  //glViewport(0, 0, m_params.buffer_width, m_params.buffer_height);
		OpenGLRenderer::SetViewport(m_params.buffer_width, m_params.buffer_height);

		glfwSetWindowUserPointer(m_window, this);  //Used for setup of event handlers
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

	Window* Window::Create(const WindowParams& initial_params)
	{
		auto window = new Window(initial_params);
		return window;
	}

	void Window::ShutDown()
	{ 
			glfwSetWindowShouldClose(m_context->GetWindowHandle(), GL_TRUE);
	}

	void Window::Clear()
	{
		const glm::vec4& col = m_params.clear_colour;
		//OpenGLRenderer::SetClearColor(col);
		//OpenGLRenderer::ClearBuffers();

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