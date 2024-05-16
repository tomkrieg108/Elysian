#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include <glm/glm.hpp>

/*
TODO---------------------------

Can setup 'hot reloading' of shader on the fly
class names should be prefixed with OpenGL
*/

/*
TODO use consistent naming for GLSL shader names (attributes, uniforms, the name of the shader itself) and
buffer layouts, materials etc
*/

namespace ely
{
	class Shader;
	class OpenGLUniformBuffer;

	enum class ShaderType : uint32_t
	{
		Vertex, Fragment, Geometry, TesselationControl, TesselationEvaluation, Compute, Unknown
	};

	class ShaderBuilder
	{
	public:
		ShaderBuilder() = default;
		~ShaderBuilder() = default;
		ShaderBuilder& Add(const ShaderType type, const std::string& filename);
		ShaderBuilder& Add(const std::string& filename);
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
		ShaderInfo Compile(uint32_t type, const std::string& filepath, const std::string& source_code);
		void OutputShaderInfoLog();

	private:
		std::vector<ShaderInfo> m_shader_list;
	};

	//------------------------------------------------------------------------------------------------

	class ShaderSource
	{
		using ShaderSourceFile = std::pair<ShaderType, std::string>; //2nd is the source filename
	public:
		ShaderSource(std::initializer_list<ShaderSourceFile> source_files);
		ShaderSource() {}
		void Add(ShaderSourceFile source_file);
		void Reset() { src.clear(); }

		//TODO: range based for loops still don't work with these!?
		std::vector<ShaderSourceFile>::const_iterator begin() const { return std::cbegin(src); };
		std::vector<ShaderSourceFile>::const_iterator end() const { return std::cend(src); };
	private:
		std::vector<ShaderSourceFile> src;
		friend class Shader;
	};

	//------------------------------------------------------------------------------------------------
	
	class Shader
	{
		friend class ShaderBuilder;

	public:

		//Attribute or uniform
		struct DataItem
		{
			std::string name = "";
			int32_t location = -1;
			ShaderDataType type = ShaderDataType::Float;
			int32_t size = 0;
		};

		struct UniformBlock
		{
			std::string name = "";
			int32_t index = -1;
			int32_t binding = -1;
			int32_t size = 0;
			int32_t active_uniforms = 0;
		};

	public:

		Shader() = default;
		~Shader() = default;

		void Bind() const;
		void Unbind() const;

		//Set uniforms
		void SetUniform1f(const std::string& name, float v0);
		void SetUniform2f(const std::string& name, float v0, float v1);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform2f(const std::string& name, const glm::vec2& data);
		void SetUniform3f(const std::string& name, const glm::vec3& data);
		void SetUniform4f(const std::string& name, const glm::vec4& data);
		void SetUniform1i(const std::string& name, int v0);
		void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

		uint32_t GetProgramID() const { return m_program_id; }
		std::string GetName() const { return m_name; }
		bool BuildSuccess() const { return m_build_success; }
		void OutputInfo();

		const auto& GetUniformData() const { return m_uniforms; }
		const auto& GetUniformBlockData() const { return m_uniform_blocks; }
		const auto& GetAttributeData() const { return m_attributes; }

		std::optional<std::reference_wrapper<const DataItem>> GetUniform(const std::string& name) const;
		std::optional<std::reference_wrapper<const UniformBlock>> GetUniformBlock(const std::string& name) const;
		std::optional<std::reference_wrapper<const DataItem>> GetAttribute(const std::string& name) const;

		static Ref<Shader> Create(const std::string& filename, const std::string& name);			//For a single file
		static Ref<Shader> Create(const ShaderSource& shader_source, const std::string& name);	//If split into multiple files
		
	private:
		std::string m_name = "Unnamed Shader";
		bool m_build_success = false;
		uint32_t m_program_id = 0;
		std::unordered_map<std::string, int32_t> m_uniform_location_cache;  //TODO:  don't need this since querying and storing the uniforms in m_uniforms on initialisation
		std::unordered_map<std::string, DataItem> m_attributes;
		std::unordered_map<std::string, DataItem> m_uniforms;
		std::unordered_map<std::string, UniformBlock> m_uniform_blocks;


	private:
		
		int32_t GetUniformLocation(const std::string& name);
		void ReadUniforms();
		void ReadUniformBlocks();
		void ReadAttributes();
	};

	//------------------------------------------------------------------------------------------------
	//TODO - make as a template class? - similar type of thing for textures, models etc
	//TODO - use a 'service locator' thing?
	class ShaderRepo
	{
	public:
		static void Init();
		static Ref<Shader> Load(const ShaderSource& shader_source, const std::string& shader_name);
		static Ref<Shader> Load(const std::string& filename, const std::string& shader_name);
		static Ref<Shader> Get(const std::string& name);
		static bool Exists(const std::string& name);
		static auto& GetShaders() { return m_shader_repo; }

		static auto begin() { return std::begin(m_shader_repo); }
		static auto end() { return std::end(m_shader_repo); }

		//static const OpenGLUniformBuffer& GetUniformBuffer(const std::string& name);
		static std::optional <std::reference_wrapper<OpenGLUniformBuffer>> GetUniformBuffer(const std::string& name);
		
	private:
		static std::unordered_map<std::string, Ref<Shader>> m_shader_repo;
		static std::unordered_map<std::string, OpenGLUniformBuffer> m_uniform_buffers;
		static const std::string s_shader_asset_path;

		friend ShaderBuilder;
	};

	class ShaderHandle
	{
	public:
		ShaderHandle() = delete;
		ShaderHandle(const Shader& shader) : 
			m_program_id{ shader.GetProgramID() },
			m_shader{shader}
		{ }
		const Shader& GetShader() const { return m_shader; }
		explicit operator uint32_t () const { return m_program_id; }
	private:
		uint32_t m_program_id = 0;
		const Shader& m_shader;
	};
}