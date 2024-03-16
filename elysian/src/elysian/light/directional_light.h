#pragma once

#include "elysian/renderer/opengl_shader.h"
#include <glm/glm.hpp>

namespace ely
{
	class DirectionalLight
	{
		// This is a simple directional light
	public:
		DirectionalLight() = default;
		~DirectionalLight() = default;
		DirectionalLight(glm::vec3& position, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular) :
			m_position{ position }, m_ambient{ ambient }, m_diffuse{ diffuse }, m_specular{ specular }
		{}

		const glm::vec3& GetPosition() const { return m_position; }
		const glm::vec3& GetAmbient() const { return m_ambient; }
		const glm::vec3& GetDiffuse() const { return m_diffuse; }
		const glm::vec3& GetSpecular() const { return m_specular; }

		void UploadDataToShader(const Ref<Shader>& shader) const;
		void ValidateAgainstShader(const Ref<Shader>& shader) const;

	private:

		//light parameters
		glm::vec3 m_position = glm::vec3(1.2f, 1.0f, 2.0f);
		glm::vec3 m_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		glm::vec3 m_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec3 m_specular = glm::vec3(1.0f, 1.0f, 1.0f);
	};


}