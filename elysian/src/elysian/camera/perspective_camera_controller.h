#pragma once
#include "elysian/camera/perspective_camera.h"
#include "elysian/events/events.h"
#include <cmath>
#include <glm/glm.hpp>

namespace ely
{
	class Window;

	///////////////////////////////////////////////////////////
	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController() = delete;
		PerspectiveCameraController(float aspect_ratio);
		PerspectiveCameraController(float aspect_ratio, const glm::vec3& camera_pos);
		~PerspectiveCameraController() = default;

		void MoveForward(float amount);
		void MoveRight(float amount);
		void MoveVertically(float amount);
		void Turn(float amount_x, float amount_y);
		void InvertPitch();
		void Zoom(float amount);

		const PerspectiveCamera& GetCamera() const { return m_camera; }
		void OnUpdate(double delta_time);
		void OnMouseMoved(EventMouseMoved& e);
		void OnMouseScrolled(EventMouseScrolled& e);
		void OnWindowResize(EventWidowResize& e);
		void OnMouseButtonPressed(EventMouseButtonPressed& e);

	private:
		//angle in degrees
		glm::mat4 GetRotationMatX(float angle) const;
		glm::mat4 GetRotationMatY(float angle) const;
		glm::mat4 GetRotationMatZ(float angle) const;

	private:
		PerspectiveCamera m_camera;
	};

}