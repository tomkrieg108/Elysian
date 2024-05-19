#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "material.h"

namespace ely
{
	inline namespace material_v1 {

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

		void MaterialSpecification::ValidateAgainstShader(const Ref<Shader>& shader) const
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

		void MaterialData::ValidateAgainstSpec(const MaterialSpecification& spec) const
		{
			//TODO
		}

		//------------------------------------------------------------------------------

		void Material::UploadDataToShader(const Ref<Shader>& shader) const
		{
			//TODO only want to upload anything that has changed

			shader->Bind();

			/*
				Note: for range-based for loops to work here, class MaterialData needs to supply *non-const* begin() and end() itr getters,
				If this method is const then will generate a compile error
			*/


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

		void Material::ValidateAgainstShader(const Ref<Shader>& shader) const
		{
			//TODO
		}

		void Material::UploadDataToShader(Shader& shader) const
		{
			for (const auto& data_element : m_data)
			{
				const std::string& uniform_name = data_element.first;
				const MatData value = data_element.second;

				if (std::holds_alternative<OpenGLTexture2D*>(value))
				{
					auto texture = std::get<OpenGLTexture2D*>(value);
					texture->Bind();
					shader.SetUniform1i(uniform_name, texture->GetSlot());
				}

				else if (std::holds_alternative<float>(value))
					shader.SetUniform1f(uniform_name, std::get<float>(value));

				else if (std::holds_alternative<glm::vec3>(value))
					shader.SetUniform3f(uniform_name, std::get<glm::vec3>(value));

				else if (std::holds_alternative<glm::vec4>(value))
					shader.SetUniform4f(uniform_name, std::get<glm::vec4>(value));

				//TODO ASSERTION
			}
		}

		void Material::ValidateAgainstShader(const Shader& shader) const
		{
			//Note: assumes the shader is already bound
			//TODO only want to upload anything that has changed
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
			// For 'corrds' shader and 'white' shader
			//-------------------------------------------------
			s_material_repo["empty"] = CreateRef<Material>();

			//-------------------------------------------------
			// For 'colored_basic' shader
			//-------------------------------------------------
			MaterialSpecification colored_basic_spec =
			{
				{"u_object_color", ShaderDataType::Float3}
			};
			MaterialData colored_basic_yellow;
			colored_basic_yellow.SetValue("u_object_color", glm::vec3(1.0f, 1.0f, 0.0f));
			s_material_repo["colored_basic_yellow"] = CreateRef<Material>(colored_basic_spec, colored_basic_yellow);

			//-------------------------------------------------
			// For 'light_map_diff_spec' shader
			//-------------------------------------------------
			MaterialSpecification material_spec =
			{
				{"u_material.diffuse", ShaderDataType::Sampler2D},
				{"u_material.specular", ShaderDataType::Sampler2D},
				{"u_material.shininess", ShaderDataType::Float}
			};

			auto diffuse_tex_map = ely::Texture2DRepo::Get("container2.png").get();
			auto specular_tex_map = ely::Texture2DRepo::Get("container2_specular.png").get();

			MaterialData material_data;
			material_data.SetValue("u_material.diffuse", diffuse_tex_map)
				.SetValue("u_material.specular", specular_tex_map)
				.SetValue("u_material.shininess", 32.0f);


			s_material_repo["container2_specular"] = CreateRef<Material>(material_spec, material_data);
		}

	}

	namespace material_v2 {

		Material::Material() :
			m_shader{ ShaderRepo::Get("basic_diffuse") }
		{
			InitData();
			m_shader_id = m_shader->GetProgramID();
		}

		Material::Material(const Ref<Shader>& shader) :
			m_shader(shader)
		{
			//TODO - assert shader not null and is valid
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
					material_uniform.item = item.second;

					switch (item.second.type)
					{
						case ShaderDataType::Bool: material_uniform.value = false; break;
						case ShaderDataType::Float: material_uniform.value = 0.0f; break;
						case ShaderDataType::Int: material_uniform.value = 0; break;
						case ShaderDataType::Float2: material_uniform.value = glm::vec2{ 0.0f }; break;
						case ShaderDataType::Float3: material_uniform.value = glm::vec3{ 0.0f }; break;
						case ShaderDataType::Float4: material_uniform.value = glm::vec4{ 0.0f }; break;
						case ShaderDataType::Mat3: material_uniform.value = glm::mat3{ 1.0f };  break;
						case ShaderDataType::Mat4: material_uniform.value = glm::mat4{ 1.0f };  break;
						case ShaderDataType::Sampler2D: material_uniform.value = -1;  break;
						case ShaderDataType::SamplerCube: material_uniform.value = -1;  break;
						default:
						{
							CORE_ERROR(" Material::InitData(): unsupported shader data type {}: ", ShaderUtils::ShaderDataTypeToString(item.second.type));
							material_uniform.value = -1;
						}
					}

					AddItem(material_uniform);

				}

			}

		}
	}
}