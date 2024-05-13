#pragma once
#include "elysian/camera/camera.h"
#include "elysian/events/events.h"
#include "elysian/scene/entity.h"
#include <cmath>
#include <glm/glm.hpp>



namespace ely
{
	class CameraController
	{
	public:
		//TODO - this maybe in the scene class rather than cam controller
		enum class InputMode
		{
			Select, RotateWorld, RotateLocal, TranslateXY, TranslateZ
		};

	public:
		CameraController() = default;
		~CameraController() = default;

		void MoveForward(float amount);
		void MoveRight(float amount);
		void MoveVertically(float amount);
		void Turn(float delta_yaw, float delta_pitch);
		void RotateLocal(float delta_yaw, float delta_pitch);
		void RotateWorld(float amount_x, float amount_y);
		void RotateWorld2(float amount_x, float amount_y);

		//TODO - in the transform component?
		void UpdateTransform(float delta_yaw, float delta_pitch, glm::mat4& transform);

		void InvertPitch();

		//InputMode GetInputMode() const { return m_input_mode; }

		void OnUpdate(double delta_time);
		void OnMouseMoved(EventMouseMoved& e);
		void OnMouseScrolled(EventMouseScrolled& e);

		const auto& GetCameraEntity() const { return m_camera_entity; }
		void SetCameraEntity(Entity camera_entity) { m_camera_entity = camera_entity; }
		void SetGridEntity(Entity grid_entity) { m_grid_entity = grid_entity; }

	private:
		//TODO - this stuff should be somewhere else!
		//angle in degrees
		glm::mat4 GetRotationMatX(float angle) const;
		glm::mat4 GetRotationMatY(float angle) const;
		glm::mat4 GetRotationMatZ(float angle) const;

	private:
		Entity m_camera_entity, m_grid_entity; //both set by scene.cpp rather than in ctr - bit dodgy?
		InputMode m_input_mode = InputMode::Select;
	};
}