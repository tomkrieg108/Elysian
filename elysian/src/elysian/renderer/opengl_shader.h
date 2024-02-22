#pragma once
#include "elysian/kernal/base.h"
#include <glm/glm.hpp>

/*
TODO---------------------------
check to see if any of the shader programs have been built already before creating.
If it has been then return that shader

redo the validation check function and program info function - see Anton (shaders chapter)

Can setup 'hot reloading' of shader on the fly
*/

namespace ely
{
	class Shader;

	enum class ShaderType : uint32_t
	{
		Vertex, Fragment, TesselationControl, TesselationEvaluation, Geometry, Compute
	};

	class ShaderBuilder
	{
	public:
		ShaderBuilder() = default;
		~ShaderBuilder() = default;
		ShaderBuilder& Add(const ShaderType type, const std::string& filename);
		Ref<Shader> Build(const std::string& name);
		static uint32_t GetOpenGLShaderType(ShaderType type);

	private:

		struct ShaderInfo
		{
			std::string filepath{ "" };
			uint32_t type = 0;
			uint32_t id = 0;
			bool compile_success = false;
		};

	private:
		std::string ReadSource(const std::string& filepath);
		ShaderInfo Compile(uint32_t type, const std::string& filepath);
		void OutputShaderInfoLog();

	private:
		std::vector<ShaderInfo> m_shader_list;
	};

	//------------------------------------------------------------------------------------------------

	class ShaderSource
	{
		using ShaderSourceFile = std::pair<ShaderType, std::string>; //2nd is the source filename
		friend class Shader;
	public:
		ShaderSource(std::initializer_list<ShaderSourceFile> source_files);
		void Add(ShaderSourceFile source_file);
		void Reset() { src.clear(); }

		//TODO: range based for loops still don't work with these!?
		std::vector<ShaderSourceFile>::const_iterator cbegin() const { return std::cbegin(src); };
		std::vector<ShaderSourceFile>::const_iterator cend() const { return std::cend(src); };
	private:
		std::vector<ShaderSourceFile> src;
	};

	//------------------------------------------------------------------------------------------------

	class Shader
	{
		friend class ShaderBuilder;

	public:

		Shader() = default;
		~Shader() = default;

		void Bind();
		void Unbind();

		//Set uniforms
		void SetUniform1f(const std::string& name, float v0);
		void SetUniform2f(const std::string& name, float v0, float v1);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform2f(const std::string& name, glm::vec2& data);
		void SetUniform3f(const std::string& name, glm::vec3& data);
		void SetUniform4f(const std::string& name, glm::vec4& data);
		void SetUniform1i(const std::string& name, int v0);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

		uint32_t GetProgramID() const { return m_program_id; }
		std::string GetName() const { return m_name; }
		bool BuildSuccess() const { return m_build_success; }
		void OutputInfo();

		//static Ref<Shader> Create(const std::string& filepath, const std::string& name);
		static Ref<Shader> Create(const ShaderSource& shader_source, const std::string& name);	//if split into multiple files


	private:

		int32_t GetUniformLocation(const std::string& name);

		struct DataItem
		{
			std::string name;
			int location;
			uint32_t gl_type;
		};

	private:
		std::string m_name = "";
		bool m_build_success = false;
		uint32_t m_program_id = 0;
		std::unordered_map<std::string, int> m_uniform_location_cache;  //TODO:  don't need this since querying and storing the uniforms on init
		std::vector<DataItem> m_attributes;
		std::vector<DataItem> m_uniforms;
		void ReadUniforms();
		void ReadAttributes();
	};

	//------------------------------------------------------------------------------------------------

	class ShaderRepo
	{
	public:
		ShaderRepo() = default;
		~ShaderRepo() = default;

		static void Init();
		static Ref<Shader> Load(const ShaderSource& shader_source, const std::string& name);
		static Ref<Shader> Get(const std::string& name);
		static bool Exists(const std::string& name);
		
	private:
		static void LoadDefaultShaders();
		static std::unordered_map<std::string, Ref<Shader>> m_shader_repo;
		static const std::string s_shader_asset_path;

		friend ShaderBuilder;
	};

	//------------------------------------------------------------------------------------------------

	const std::string GLTypeToString(uint32_t type);
	void DisplayUniformValue(uint32_t program, int location, uint32_t type);

}