#include "pch.h"
#include "elysian/kernal/base.h"
#include <GLFW\glfw3.h>
#include "elysian/kernal/input.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/kernal/application.h"


//TODO - make input a static class

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

		events_v1::EventMouseMoved e{ m_last_x, m_last_y, m_delta_x, m_delta_y };
		events_v1::EventDispatcher::Dispatch(e);
	}

	void Input::MouseButtonPressed(int button, int action, int mods) const
	{
		//TODO: action (press or release) not needed in e 
		if (action == GLFW_PRESS)
		{
			events_v1::EventMouseButtonPressed e{ m_last_x, m_last_y, button, action };
			events_v1::EventDispatcher::Dispatch(e);
		}
		if (action == GLFW_RELEASE)
		{
			events_v1::EventMouseButtonReleased e{ m_last_x, m_last_y, button, action };
			events_v1::EventDispatcher::Dispatch(e);
		}
	}

	void Input::MouseScrolled(double xoffset, double yoffset) const
	{
		events_v1::EventMouseScrolled e{ float(xoffset), float(yoffset) };
		events_v1::EventDispatcher::Dispatch(e);
	}

	void Input::KeyAction(int key, int code, int action, int mode) const
	{
		if (action == GLFW_PRESS)
		{
			events_v1::EventKeyPressed e{ key };
			events_v1::EventDispatcher::Dispatch(e);
		}
		else if (action == GLFW_RELEASE)
		{
			events_v1::EventKeyReleased e{ key };
			events_v1::EventDispatcher::Dispatch(e);
		}
		else if (action == GLFW_REPEAT)
		{
			//TODO
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
