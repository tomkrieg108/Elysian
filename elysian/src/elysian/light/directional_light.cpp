#include "pch.h"
#include "elysian/kernal/base.h"
#include "directional_light.h"

namespace ely
{
	void DirectionalLight::UploadDataToShader(const Ref<Shader>& shader) const
	{
		shader->Bind();

		shader->SetUniform3f("u_light.position", m_position);
		shader->SetUniform3f("u_light.ambient", m_ambient);
		shader->SetUniform3f("u_light.diffuse", m_diffuse);
		shader->SetUniform3f("u_light.specular", m_specular);
	}

	void DirectionalLight::ValidateAgainstShader(const Ref<Shader>& shader) const 
	{
		//TODO
	} 

}