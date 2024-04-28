#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/kernal/window.h"
#include "elysian/kernal/input.h"
#include "elysian/events/events.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_renderer.h"

namespace ely
{

	uint32_t Window::s_window_count = 0;

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
		m_params.monitor_aspect_ratio = (float)m_params.width / (float)m_params.height;

		auto m_window = glfwCreateWindow(params.width, params.height, params.title.c_str(), NULL, NULL); //TODO - title not displayed
		if (!m_window)
		{
			CORE_ERROR("GLFW window creation failed");
			glfwTerminate();
		}
	
		m_context = new OpenGLContext(m_window);
		m_context->Init();
		
		glfwGetFramebufferSize(m_window, &m_params.buffer_width, &m_params.buffer_height);
		OpenGLRenderer::SetViewport(m_params.buffer_width, m_params.buffer_height);

		SetVSync(true);
		glfwSetWindowUserPointer(m_window, this);  //Used for setup of event handlers
		SetCursorEnabled(true);
		CORE_INFO("WINDOW CREATED:");
		CORE_TRACE("   Width: {}, Height: {}, Buff Width: {}, Buff Height: {}", m_params.width, m_params.height,
			m_params.buffer_width, m_params.buffer_height);

		//set callbacks---------------

		//Windows Events
		//---------------------------------------------

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventWidowClose event;
			auto& callback_func = win->GetEventCallback();
			callback_func(event);
			});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			win->m_params.width = width;
			win->m_params.height = height;
			//update window framebuffer width and height (excludes window border and title bar)
			glfwGetFramebufferSize(win->GetWindowHandle(), &(win->m_params.buffer_width), &(win->m_params.buffer_height));

			//TODO - should this be somewhere else?
			OpenGLRenderer::SetViewport(win->m_params.buffer_width, win->m_params.buffer_height);
			EventWidowResize event{ (uint32_t)win->m_params.buffer_width, (uint32_t)win->m_params.buffer_height };
			auto& callback_func = win->GetEventCallback();
			callback_func(event); //event is std::placeholder::_1 in MakeCallback.  TODO. I like EventDispatcher::Dispatch(e); better!!
			});

		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			//CORE_INFO("Frame buffer resized");
			});


		glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, int entered) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventWindowHoverChange event{ entered };
			auto& callback_func = win->GetEventCallback();
			callback_func(event);
			});

		glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int xpos, int ypos) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventWindowMove event{ (uint32_t)xpos, (uint32_t)ypos };
			auto& callback_func = win->GetEventCallback();
			callback_func(event);
			});

		glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventWindowFocusChange event{ focused };
			auto& callback_func = win->GetEventCallback();
			callback_func(event);
			});

		glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventWindowIconifyChange event{ iconified };
			auto& callback_func = win->GetEventCallback();
			callback_func(event);
			});

		glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximized) {
			//CORE_INFO("Window maximized");
			});

		//TODO: there is also a viewport event defined which is dispated in the editor - not ideal!

		//Key Events
		//---------------------------------------------
		glfwSetKeyCallback(m_window, [](GLFWwindow* glfw_window, int key, int code, int action, int mode) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
			if (action == GLFW_PRESS)
				win->GetEventCallback()(EventKeyPressed{ key });
			else if (action == GLFW_RELEASE)
				win->GetEventCallback()(EventKeyReleased{ key });
			else if (action == GLFW_REPEAT)
				win->GetEventCallback()(EventKeyPressed{ key, true });
			});

		glfwSetCharCallback(m_window, [](GLFWwindow* glfw_window, unsigned int keycode)
			{
				//CORE_INFO("Char typed");
				Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
				EventKeyTyped event(keycode);
				auto& callback_func = win->GetEventCallback();
				callback_func(event);
			});


		//Mouse Events
		//---------------------------------------------

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			auto& callback = win->GetEventCallback();

			if (action == GLFW_PRESS)
				callback(EventMouseButtonPressed{ Input::GetMouseX(), Input::GetMouseY(), button });
			if (action == GLFW_RELEASE)
				callback(EventMouseButtonReleased{ Input::GetMouseX(), Input::GetMouseY(), button });
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
			EventMouseScrolled event{ float(xoffset), float(yoffset) };
			auto& callback = win->GetEventCallback();
			callback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		
			if (!Input::GetMouseFirstMoved())
				Input::SetMouseFirstMoved();

			auto& callback = win->GetEventCallback();
			EventMouseMoved event{ Input::GetMouseDeltaX((float)xpos), Input::GetMouseDeltaY((float)ypos), };
			callback(event);
		});

	}

	bool Window::IsMinimised() const
	{
		return (bool)glfwGetWindowAttrib(m_context->GetWindowHandle(), GLFW_ICONIFIED);
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

	void Window::Clear()
	{
		const glm::vec4& col = m_params.clear_colour;
		OpenGLRenderer::SetClearColor(col);
		OpenGLRenderer::ClearBuffers();
	}

	//TODO this stuff should be in the renderer API ? 
	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_context->GetWindowHandle()); //TODO:  use the call in opengl_context
	}

	void Window::SetVSync(bool enabled)
	{
		enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
		m_params.vsync_enabled = enabled;
	}

	bool Window::IsVSyncEnabled() const
	{
		return m_params.vsync_enabled;
	}

	void Window::ErrorCallback(int error, const char* description)
	{
		CORE_ERROR("GLFW Error: {}", description);
	}
	
}