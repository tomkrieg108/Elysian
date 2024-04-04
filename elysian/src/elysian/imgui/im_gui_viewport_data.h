#pragma once

#include <glm/glm.hpp>

namespace ely {

	struct ImGuiViewPortData
	{
		glm::vec2 size{ 0,0 };
		glm::vec2 window_pos{ 0,0 };
		glm::vec2 viewport_pos{ 0,0 };
		glm::vec2 cursor_screen_pos{ 0, 0 };		//The position of the main viewport into which content goes
		glm::vec2 mouse_pos{ 0,0 };
		glm::vec2 mouse_pos_viewport{ 0, 0 };						//The position of the mouse cursor relative to main viewport
		bool forcused = false;
		bool hovered = false;
	};
}