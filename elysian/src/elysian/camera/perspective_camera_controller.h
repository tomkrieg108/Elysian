#pragma once
#include "elysian/camera/perspective_camera.h"
#include "elysian/events/events.h"
#include "elysian/scene/entity.h"
#include <cmath>
#include <glm/glm.hpp>

//class Window;

namespace ely
{
	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController() = default;

		void MoveForward(float amount);
		void MoveRight(float amount);
		void MoveVertically(float amount);
		void Turn(float amount_x, float amount_y);
		void InvertPitch();
		void Zoom(float amount);

		const auto& GetCameraEntity() const { return m_camera_entity; }
		void SetCameraEntity(Entity camera_entity) { m_camera_entity = camera_entity; }

		void OnUpdate(double delta_time);
		void OnMouseMoved(EventMouseMoved& e);
		void OnMouseScrolled(EventMouseScrolled& e);
		void OnWindowResize(EventWidowResize& e);
		void OnViewportResize(EventViewportResize& e);
		void OnMouseButtonPressed(EventMouseButtonPressed& e);

	private:
		//angle in degrees
		glm::mat4 GetRotationMatX(float angle) const;
		glm::mat4 GetRotationMatY(float angle) const;
		glm::mat4 GetRotationMatZ(float angle) const;

	private:
		Entity m_camera_entity;
	};
}