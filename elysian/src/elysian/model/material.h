#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include <variant>

/*
TODO use consistent naming for GLSL shader names (attributes, uniforms, the name of the shader itself) and
buffer layouts, materials etc
*/


namespace ely
{
	class MaterialSpecification
	{
	public:

		MaterialSpecification() = default;

		//Supply spec as (key, value) pair initializer list, with key the name of a shader uniform, value it's data type
		MaterialSpecification(std::initializer_list<std::pair<const std::string, ShaderDataType>> material_spec) :
			m_material_spec{ material_spec }
		{}

		//Determine specification from shader	
		MaterialSpecification(const Ref<Shader>& shader);

		MaterialSpecification& AddSpecElement(const std::string& name, ShaderDataType&& element);
		void ValidateAgainstShader(const Ref<Shader>& shader);

		auto cbegin() const { return std::cbegin(m_material_spec); }
		auto cend() const { return std::cend(m_material_spec); }
		auto begin()  { return std::begin(m_material_spec); }
		auto end()  { return std::end(m_material_spec); }

	private:
		std::unordered_map<std::string, ShaderDataType> m_material_spec;
	};

	using MatData = std::variant<uint32_t, float, glm::vec3, glm::vec4, OpenGLTexture2D*>;
	
	class MaterialData
	{
	public:
		MaterialData() = default;

		MaterialData(std::initializer_list<std::pair<const std::string, MatData>> material_data) :
			m_material_data{ material_data }
		{}

		MaterialData& SetValue(const std::string& name, MatData&& element);
		MaterialData& SetValue(const std::string& name, const MatData& element);
		const MatData& GetValue(const std::string& name);
		void ValidateAgainstSpec(const MaterialSpecification& spec);

		auto cbegin() const { return std::cbegin(m_material_data); }
		auto cend() const { return std::cend(m_material_data); }
		auto begin()  { return std::begin(m_material_data); }
		auto end()  { return std::end(m_material_data); }

	private:
		std::unordered_map<std::string, MatData> m_material_data;
	};

	class Material
	{
	public:
		
		Material() = default;

		Material(MaterialSpecification& specification, const std::string name = "Material") :
			m_specification{ specification }, m_name{ name }
		{}

		Material(MaterialSpecification& specification, MaterialData& data, const std::string name = "Material") :
			m_specification{ specification }, m_data{ data }, m_name{ name }
		{}

		void UploadDataToShader(const Ref<Shader>& shader);
		void ValidateAgainstShader(const Ref<Shader>& shader);

		void SetValue(const std::string& name, MatData&& element) { m_data.SetValue(name, element); }
		const MatData& GetValue(const std::string& name) { m_data.GetValue(name); } //TODO check that value exists

		const MaterialData& GetData() const { return m_data; }
		const MaterialSpecification& GetSpecification() const { return m_specification; }

	private:
		std::string m_name = "Empty Material";
		MaterialSpecification m_specification;
		MaterialData m_data;
	};



	//-----------------------------------------------------------------------------------
	//TODO - make as a template class?
	//NOTE: this depends on the texture 2D repo, so it must ne initialised after that 
	class MaterialRepo
	{
	public:
		static void Init();
		static void Load(const std::string& name);
		static Ref<Material> Get(const std::string& name);
		static bool Exists(const std::string& name);

	private:
		static void LoadMaterials();
		static std::unordered_map<std::string, Ref<Material>> s_material_repo;
	};

	//TODO - make a seperate repo for MaterialSpecification ?

	//Make the MaterialSpecification name the same as the corresponding shader?
	//The Material name same as corresponding shader with a postfix id of some kind?
}