#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_shader.h"
#include <variant>

namespace ely
{
	class MaterialSpecification
	{
	public:

		//Supply spec as (key, value) pair initializer list, with key the name of a shader uniform, value it's data type
		MaterialSpecification(std::initializer_list<std::pair<const std::string, ShaderDataType>> material_spec) :
			m_material_spec{ material_spec }
		{}

		//Determine specification from shader
		MaterialSpecification(const Shader& shader) {}

		MaterialSpecification& AddSpecElement(const std::string& name, ShaderDataType&& element)
		{
			//TODO check that name not already added
			m_material_spec[name] = element;
			return *this;
		}

		void ValidateAgainstShader(const Shader& shader) {}

	private:
		std::unordered_map<std::string, ShaderDataType> m_material_spec;
	};

	//using MatData = std::variant<uint32_t, float, glm::vec3, glm::vec4>;
	using MatData = std::variant<uint32_t, float>;

	class MaterialData
	{
	public:
		MaterialData() {}
		MaterialData(std::initializer_list<std::pair<const std::string, MatData>> material_data) :
			m_material_data{ material_data }
		{}

		MaterialData& SetValue(const std::string& name, MatData&& element)
		{
			m_material_data[name] = element;
			return *this;
		}
		MaterialData& SetValue(const std::string& name, const MatData& element)
		{
			m_material_data[name] = element;
			return *this;
		}

		void ValidateAgainstSpec(const MaterialSpecification& spec) {}

	private:
		std::unordered_map<std::string, MatData> m_material_data;
	};

	class Material
	{
	public:
		Material(const Ref<Shader>& shader, MaterialSpecification& specification) :
			m_shader{ shader }, m_specification{ specification }
		{}

		Material(const Ref<Shader>& shader, MaterialSpecification& specification, MaterialData& data) :
			m_shader{ shader }, m_specification{ specification }, m_data{ data }
		{}

		void SetValue(const std::string& name, MatData&& element)
		{
			m_data.SetValue(name, element);
		}

	private:
		const Ref<Shader>& m_shader;
		MaterialSpecification m_specification;
		MaterialData m_data;
	};
}