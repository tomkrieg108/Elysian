#pragma once
#include <GL\glew.h>
#include "elysian/kernal/base.h"
#include <glm\glm.hpp>

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
		Vertex									=	GL_VERTEX_SHADER,
		Fragment								=	GL_FRAGMENT_SHADER,
		TesselationControl			=	GL_TESS_CONTROL_SHADER,
		TesselationEvaluation		= GL_TESS_EVALUATION_SHADER,
		Geometry								=	GL_GEOMETRY_SHADER,
		Compute									= GL_COMPUTE_SHADER
	};

	class ShaderBuilder
	{
	public:

		ShaderBuilder() = default;
		ShaderBuilder& BuildShader(const ShaderType type, const std::string& filename);
		~ShaderBuilder() = default;

		//TODO:  should probably use a shared ptr in case there are multiple refs to the same shader used by different meshes
		std::unique_ptr<Shader> BuildProgram(const std::string& name);

	private:

		struct ShaderInfo
		{
			std::string filepath = "";
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
		static const std::string s_shader_path;
	};


	class Shader
	{
		friend class ShaderBuilder;
	public:

		Shader() = default;
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
		void ReadUniforms() ;
		void ReadAttributes();
	};

	const std::string GLTypeToString(GLenum type);
	void DisplayUniformValue(uint32_t program, int location, GLenum type);

}
