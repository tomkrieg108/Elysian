#include "pch.h"
#include "elysian/kernal/base.h"
#include "directional_light.h"

namespace ely
{
	void DirectionalLight::UploadDataToShader(const Ref<Shader>& shader) const
	{
		shader->Bind();

		shader->SetUniform3f("u_dir_light.direction", direction);
		shader->SetUniform3f("u_dir_light.color", color);
		shader->SetUniform3f("u_ambient_light", ambient_color);
	}

	void DirectionalLight::ValidateAgainstShader(const Ref<Shader>& shader) const 
	{
		//TODO
	} 

}