#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "material.h"

namespace ely
{
	MaterialSpecification::MaterialSpecification(const Ref<Shader>& shader)
	{
		//TODO
	}

	MaterialSpecification& MaterialSpecification::AddSpecElement(const std::string& name, ShaderDataType&& element)
	{
		//TODO check that name not already added
		m_material_spec[name] = element;
		return *this;
	}

	void MaterialSpecification::ValidateAgainstShader(const Ref<Shader>& shader)
	{
		//TODO
	}

	//------------------------------------------------------------------------------

	MaterialData& MaterialData::SetValue(const std::string& name, MatData&& element)
	{
		m_material_data[name] = element;
		return *this;
	}
	MaterialData& MaterialData::SetValue(const std::string& name, const MatData& element)
	{
		m_material_data[name] = element;
		return *this;
	}

	const MatData& MaterialData::GetValue(const std::string& name)
	{
		//TODO check that the value actually exists
		return m_material_data[name];
	}

	void MaterialData::ValidateAgainstSpec(const MaterialSpecification& spec) 
	{
		//TODO
	}

	//------------------------------------------------------------------------------

	void Material::UploadDataToShader(const Ref<Shader>& shader)
	{
		//TODO only want to upload anything that has changed

		shader->Bind();

		for (const auto& data_element : m_data)
		{
			const std::string& uniform_name = data_element.first;
			const MatData value = data_element.second;

			if (std::holds_alternative<OpenGLTexture2D*>(value))
			{
				auto texture = std::get<OpenGLTexture2D*>(value);
				texture->Bind();
				shader->SetUniform1i(uniform_name, texture->GetSlot());
			}
				
			else if (std::holds_alternative<float>(value))
				shader->SetUniform1f(uniform_name, std::get<float>(value));

			else if (std::holds_alternative<glm::vec3>(value))
				shader->SetUniform3f(uniform_name, std::get<glm::vec3>(value));

			else if (std::holds_alternative<glm::vec4>(value))
				shader->SetUniform4f(uniform_name, std::get<glm::vec4>(value));

			//TODO ASSERTION
		}
	}

	void Material::ValidateAgainstShader(const Ref<Shader>& shader)
	{
		//TODO
	}

	//-------------------------------------------------------------------------------------

	std::unordered_map<std::string, Ref<Material>> MaterialRepo::s_material_repo{};

	void MaterialRepo::Init()
	{
		MaterialRepo::LoadMaterials();
	}

	void MaterialRepo::Load(const std::string& name)
	{
		//TODO: assert that texture doesn't already exist
		//s_material_repo[name] = CreateRef<Material>(name);
	}

	Ref<Material> MaterialRepo::Get(const std::string& name)
	{
		//TODO: assert that texture exists
		return s_material_repo[name];
	}

	bool MaterialRepo::Exists(const std::string& name)
	{
		return s_material_repo.find(name) != s_material_repo.end();
	}

	void MaterialRepo::LoadMaterials()
	{
		//-------------------------------------------------
		// For 'light_map_diff_spec' shader
		//-------------------------------------------------
		MaterialSpecification material_spec =
		{
			{"material.diffuse", ShaderDataType::Sampler2D},
			{"material.specular", ShaderDataType::Sampler2D},
			{"material.shininess", ShaderDataType::Float}
		};

		auto diffuse_tex_map = ely::Texture2DRepo::Get("container2.png").get();
		auto specular_tex_map = ely::Texture2DRepo::Get("container2_specular.png").get();

		MaterialData material_data;
		material_data.SetValue("material.diffuse", diffuse_tex_map)
									.SetValue("material.specular", specular_tex_map)
									.SetValue("material.shininess", 32.0f);


		s_material_repo["container2_specular"] = CreateRef<Material>(material_spec, material_data);

		//-------------------------------------------------
		// For 'corrds' shader and 'white' shader
		//-------------------------------------------------
		s_material_repo["empty"] = CreateRef<Material>();

		//-------------------------------------------------
		// For 'colored_basic' shader
		//-------------------------------------------------
		MaterialSpecification colored_basic_spec =
		{
			{"objectColor", ShaderDataType::Float3}
		};
		MaterialData colored_basic_yellow;
		colored_basic_yellow.SetValue("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
		s_material_repo["colored_basic_yellow"] = CreateRef<Material>(colored_basic_spec, colored_basic_yellow);
	}

}