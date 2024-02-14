#pragma once
#include <glm/glm.hpp>

namespace ely
{
	class PerspectiveCamera
	{
		friend class PerspectiveCameraController;
	public:
		PerspectiveCamera() = delete;
		PerspectiveCamera(float aspect_ratio);
		PerspectiveCamera(float aspect_ratio, glm::vec3 position);
		~PerspectiveCamera() = default;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetInverseProjMatrix() const;
		const glm::mat4& GetModelMatrix() const;

		void SetAspectRatio(float width, float height);
		void SetPosition(glm::vec3& position);
		glm::vec3 GetPosition() const;
		glm::vec3 GetFront() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;

		float GetFov() const { return m_fov; }
		float GetAspectRatio() const { return m_aspect_ratio; }
		float GetNear() const { return m_near; }
		float GetFar() const { return m_far; }

	private:
		float m_aspect_ratio = 1.0f;			// = width/height
		float m_fov = 45.0f;			// Field of view - in degrees
		float m_near = 0.2f;
		float m_far = 1000.0f;
		
		glm::mat4 m_model = glm::mat4(1.0f); //itentity matrix
	};

}