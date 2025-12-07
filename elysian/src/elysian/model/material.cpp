#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "material.h"

namespace ely
{
	Material::Material() :
		m_shader{ ShaderRepo::Get("basic_diffuse") }
	{
		ASSERT(m_shader != nullptr);
		ASSERT(m_shader->BuildSuccess());
		InitData();
		m_shader_id = m_shader->GetProgramID();
	}

	Material::Material(const Ref<Shader> shader) :
		m_shader(shader)
	{
		ASSERT(m_shader != nullptr);
		ASSERT(m_shader->BuildSuccess());
		InitData();
		m_shader_id = m_shader->GetProgramID();
	}

	void Material::InitData()
	{
		const auto& uniform_data = m_shader->GetUniformData();

		for (const auto& item : uniform_data)
		{
			if (item.second.name.find("u_material") != std::string::npos)
			{
				MaterialUniform material_uniform;
				material_uniform.uniform = item.second;

				//Set default material values
				if (item.second.name.find("diffuse") != std::string::npos)
				{
					ASSERT(item.second.type == ShaderDataType::Sampler2D);
					auto default_diffuse_tex_map = ely::Texture2DRepo::Get("container2.png").get();
					ASSERT(default_diffuse_tex_map != nullptr);
					material_uniform.value = default_diffuse_tex_map;
				}
				if (item.second.name.find("specular") != std::string::npos)
				{
					ASSERT(item.second.type == ShaderDataType::Sampler2D);
					auto default_specular_tex_map = ely::Texture2DRepo::Get("container2_specular.png").get();
					ASSERT(default_specular_tex_map != nullptr);
					material_uniform.value = default_specular_tex_map;
				}
				if (item.second.name.find("shininess") != std::string::npos)
				{
					ASSERT(item.second.type == ShaderDataType::Float);
					material_uniform.value = 32.0f;
				}
				if (item.second.name.find("color") != std::string::npos)
				{
					ASSERT(item.second.type == ShaderDataType::Float3);
					material_uniform.value = glm::vec3{ 1.0f, 1.0f, 0.0f };
				}
				AddItem(material_uniform);
			}
		}
	}

		
	void Material::UploadDataToShader() const
	{
		for (const auto& item : material_data)
		{
			const std::string& uniform_name = item.uniform.name;
			const auto value = item.value;

			if (std::holds_alternative<OpenGLTexture2D*>(value))
			{
				auto texture = std::get<OpenGLTexture2D*>(value);
				texture->Bind();
				m_shader->SetUniform1i(uniform_name, texture->GetSlot());
			}

			else if (std::holds_alternative<float>(value))
				m_shader->SetUniform1f(uniform_name, std::get<float>(value));

			else if (std::holds_alternative<glm::vec3>(value))
				m_shader->SetUniform3f(uniform_name, std::get<glm::vec3>(value));
		}
	}

}