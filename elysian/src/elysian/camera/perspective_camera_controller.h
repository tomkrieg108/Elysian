#pragma once
#include "elysian/camera/perspective_camera.h"
#include "elysian/events/events.h"
#include "elysian/scene/entity.h"
#include <cmath>
#include <glm/glm.hpp>

class Window;

namespace ely
{
	class PerspectiveCameraController2
	{
	public:
		PerspectiveCameraController2() = delete;
		PerspectiveCameraController2(float aspect_ratio);
		PerspectiveCameraController2(float aspect_ratio, const glm::vec3& camera_pos);
		~PerspectiveCameraController2() = default;

		void MoveForward(float amount);
		void MoveRight(float amount);
		void MoveVertically(float amount);
		void Turn(float amount_x, float amount_y);
		void InvertPitch();
		void Zoom(float amount);

		const PerspectiveCamera2& GetCamera() const { return m_camera; }
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
		PerspectiveCamera2 m_camera;
	};
}

namespace ely
{
		class PerspectiveCameraController
		{
		public:
			PerspectiveCameraController() = default;
			PerspectiveCameraController(Ref<Entity> camera_entity) :
				//TODO assert not null
				m_camera_entity{ camera_entity }
			{}

			~PerspectiveCameraController() = default;

			void MoveForward(float amount);
			void MoveRight(float amount);
			void MoveVertically(float amount);
			void Turn(float amount_x, float amount_y);
			void InvertPitch();
			void Zoom(float amount);

			const auto& GetCameraEntity() const { return m_camera_entity; }
			void SetCameraEntity(Ref<Entity> camera_entity) { m_camera_entity = camera_entity; }

			void OnUpdate(double delta_time);
			void OnMouseMoved(EventMouseMoved& e);
			void OnMouseScrolled(EventMouseScrolled& e);
			void OnWindowResize(EventWidowResize& e);
			void OnMouseButtonPressed(EventMouseButtonPressed& e);

			//TODO - these should be in the entity class?
			auto& GetCamera()
			{
				auto& components = m_camera_entity->GetComponents();
				auto& camera_comp = std::dynamic_pointer_cast<PerspectiveCameraComponent>(components["camera"]);
				//TODO assert not null
				return camera_comp->GetCamera();
			}

			auto& GetTransform()
			{
				auto& components = m_camera_entity->GetComponents();
				auto& transform_comp = std::dynamic_pointer_cast<TransformComponent>(components["transform"]);
				//TODO assert not null
				return transform_comp->GetTransform();
			}

		private:
			//angle in degrees
			glm::mat4 GetRotationMatX(float angle) const;
			glm::mat4 GetRotationMatY(float angle) const;
			glm::mat4 GetRotationMatZ(float angle) const;

		private:
			Ref<Entity> m_camera_entity = nullptr;
		};
}

namespace ely
{
	//used for test_layer4!

	namespace scene_v2 {

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
			void SetCameraEntity(ely::scene_v2::Entity camera_entity) { m_camera_entity = camera_entity; }

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
			ely::scene_v2::Entity m_camera_entity;
		};

	}
}