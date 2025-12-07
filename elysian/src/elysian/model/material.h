#pragma once
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include <variant>

/*
TODO use consistent naming for GLSL shader names (attributes, uniforms, the name of the shader itself) and
buffer layouts, materials etc
*/


namespace ely
{
	class Material
	{
	public:

		using DataType = std::variant<float, glm::vec3, OpenGLTexture2D*>;

		struct MaterialUniform
		{
			Shader::Uniform uniform;
			DataType value;
		};

	public:
		Material();
		Material(const Ref<Shader> shader);

		auto begin() { return std::begin(material_data); }
		auto end() { return std::end(material_data); }

		void AddItem(MaterialUniform material_uniform) {
			material_data.push_back(material_uniform);
		}

		void UploadDataToShader() const;

		const Ref<Shader> GetShader() const 
		{ 
			return m_shader; 
		}
		uint32_t GetShaderId() const { return m_shader_id; }

	private:

		void InitData();

		const Ref<Shader> m_shader;
		uint32_t m_shader_id = 0;

		std::string m_material_name = "Unnamed Material";
		std::vector<MaterialUniform> material_data;
	};

}