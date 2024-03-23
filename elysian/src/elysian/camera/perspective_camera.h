#pragma once

#include <glm/glm.hpp>

namespace ely
{
		class PerspectiveCameraController2;

		class PerspectiveCamera2
		{
			friend class PerspectiveCameraController2;
		public:
			PerspectiveCamera2() = delete;
			PerspectiveCamera2(float aspect_ratio);
			PerspectiveCamera2(float aspect_ratio, glm::vec3 position);
			~PerspectiveCamera2() = default;

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

namespace ely
{
	class PerspectiveCameraController;
	
	class PerspectiveCamera
	{
		friend class PerspectiveCameraController;
		
	public:
		PerspectiveCamera();
		~PerspectiveCamera() = default;

		glm::mat4 GetViewMatrix(const glm::mat4& transform_matrix) const;
		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetInverseProjMatrix() const;
		

		void SetAspectRatio(float width, float height);

		void SetPosition(glm::vec3& position, glm::mat4& transform_matrix);

		//TODO - not sure if needed - values are in the transform component
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

		// use the seperate transform component instead of including a model matrix
	};
}