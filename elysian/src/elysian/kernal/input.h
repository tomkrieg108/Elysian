#pragma once

#include <glm/glm.hpp>

namespace ely
{
	class Input
	{
	public:
		Input();
		~Input() = default;

		void MouseMoved(double xpos, double ypos);
		glm::vec2 Input::QueryMousePosition();

		void MouseButtonPressed(int button, int action, int mods) const;
		void MouseScrolled(double xoffset, double yoffset) const;
		void KeyAction(int key, int code, int action, int mode) const;

		bool IsKeyPressed(int key) const;
		bool IsMouseButtonPressed(int button) const;

		float GetMouseX() const { return m_last_x; }
		float GetMouseY() const { return m_last_y; }
		float GetMouseDeltaX() const { return m_delta_x; }
		float GetMouseDeltaY() const { return m_delta_y; }

	private:
		float m_last_x = 0, m_last_y = 0, m_delta_x = 0, m_delta_y = 0;
		bool m_mouse_first_moved = false;
	};
}