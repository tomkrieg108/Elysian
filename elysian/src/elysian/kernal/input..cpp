#include "pch.h"
//#include <GL\glew.h>  //only so that don't get error if include application.h  gl.h included before glew.h (from including application.h)
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include "elysian/kernal/base.h"
#include "elysian/kernal/input.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/kernal/application.h"

namespace ely
{
	Input::Input()
	{
	}

	void Input::MouseMoved(double xpos, double ypos)
	{
		if (!m_mouse_first_moved)
		{
			m_mouse_first_moved = true;
			m_last_x = (float)xpos;
			m_last_y = (float)ypos;
		}

		//top left is (0,0)
		//mouse up & mouse right give pos deltas with this
		m_delta_x = (float)xpos - m_last_x;
		m_delta_y = m_last_y - (float)ypos; 

		m_last_x = (float)xpos;
		m_last_y = (float)ypos;

		EventMouseMoved e{ m_last_x, m_last_y, m_delta_x, m_delta_y };
		EventDispatcher::Dispatch(e);
	}

	void Input::MouseButtonPressed(int button, int action, int mods) const
	{
		//TODO: action (press or release) not needed in e 
		if (action == GLFW_PRESS)
		{
			EventMouseButtonPressed e{ m_last_x, m_last_y, button, action };
			EventDispatcher::Dispatch(e);
		}
		if (action == GLFW_RELEASE)
		{
			EventMouseButtonReleased e{ m_last_x, m_last_y, button, action };
			EventDispatcher::Dispatch(e);
		}
	}

	void Input::MouseScrolled(double xoffset, double yoffset) const
	{
		EventMouseScrolled e{ float(xoffset), float(yoffset) };
		EventDispatcher::Dispatch(e);
	}

	void Input::KeyAction(int key, int code, int action, int mode) const
	{
		if (action == GLFW_PRESS)
		{
			EventKeyPressed e{ key };
			EventDispatcher::Dispatch(e);
		}
		else if (action == GLFW_RELEASE)
		{
			EventKeyReleased e{ key };
			EventDispatcher::Dispatch(e);
		}
		else if (action == GLFW_REPEAT)
		{
		}
	}

	glm::vec2 Input::QueryMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetWindowHandle());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_last_x = (float)xpos;
		m_last_y = (float)ypos;
		return { (float)xpos, (float)ypos };
	}

	bool Input::IsKeyPressed(int key) const
	{
		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetWindowHandle());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(int button) const
	{
		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetWindow().GetWindowHandle());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
}
