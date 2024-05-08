#pragma once

#include "elysian/renderer/opengl_shader.h"
#include <glm/glm.hpp>

namespace ely
{
	struct DirectionalLight
	{
		//light parameters
		glm::vec3 direction{ 1.0, 1.0, 1.0 };
		glm::vec3 color{ 1.0, 1.0, 1.0 };
		glm::vec3 ambient_color = glm::vec3(0.2f, 0.2f, 0.2f);  //TODO should be an environmental param

		//TODO - should be in renderer
		void UploadDataToShader(const Ref<Shader>& shader) const;
		void ValidateAgainstShader(const Ref<Shader>& shader) const;
	};


}