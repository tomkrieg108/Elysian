#pragma once

#include <glm/glm.hpp>

namespace ely
{
	class PerspectiveCameraController;
	
	class PerspectiveCamera
	{
		friend class PerspectiveCameraController;
		
	public:
		PerspectiveCamera();
		~PerspectiveCamera() = default;

		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetInverseProjMatrix() const;
		void SetAspectRatio(float width, float height);

	
		//TODO - not sure if this stuff needed - get from the transform component
		glm::mat4 GetViewMatrix(const glm::mat4& transform_matrix) const;
		void SetPosition(glm::vec3& position, glm::mat4& transform_matrix);
		glm::vec3 GetPosition(const glm::mat4& transform_matrix) const;
		glm::vec3 GetFront(const glm::mat4& transform_matrix) const;
		glm::vec3 GetRight(const glm::mat4& transform_matrix) const;
		glm::vec3 GetUp(const glm::mat4& transform_matrix) const;

		float GetFov() const { return m_fov; }
		float GetAspectRatio() const { return m_aspect_ratio; }
		float GetNear() const { return m_near; }
		float GetFar() const { return m_far; }
		void SetFov(float fov) { m_fov = fov; }

	private:
		float m_aspect_ratio = 1.0f;			// = width/height
		float m_fov = 45.0f;							// Field of view - in degrees
		float m_near = 0.2f;
		float m_far = 1000.0f;

		//NOTE:  position etc are in a seperate transform component - not here
	};
}