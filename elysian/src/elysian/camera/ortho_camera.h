#pragma once
#include <glm/glm.hpp>

namespace ely
{
	class OrthoCamera
	{
		friend class OrthoCameraController;
	public:

		OrthoCamera() = delete;
		OrthoCamera(glm::vec4 extent);
		OrthoCamera(glm::vec4 extent, glm::vec3 position);
		~OrthoCamera() = default;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetInverseProjMatrix() const;
		const glm::mat4& GetModelMatrix() const;

		void SetPosition(glm::vec3& position);
		void SetAspectRatio(float width, float height);

		glm::vec3 GetPosition() const;
		glm::vec3 GetFront() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;

		float GetNear() const { return m_near; }
		float GetFar() const { return m_far; }

	private:
		float m_aspect_ratio = 1.0;
		float m_near = 0.2f;
		float m_far = 1000.0f;

		glm::mat4 m_model = glm::mat4(1.0f); //itentity matrix
		glm::vec4 m_extent = glm::vec4(-20.0f, 20.0f, 20.0f, -20.0f);	//left, right, top, bottom
	};
}