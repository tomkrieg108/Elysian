#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "elysian/kernal/log.h"
#include "opengl_shader_utils.h"
#include "elysian/renderer/opengl_shader.h"


//NOTE
/*
Save the uniform locations for each uniform straight after compiling the shader rather than caching them

material (according to cherno) would include a shader plus all the data that the shader needs (except lighting and camera data)

Material* material = new Material(shader)
queries all the data that the shader needs
renderer will set the uniforms related to transforms, projections, light properties etc.
material sets the uniforms related to the surface properties

2 components to shader data - stuff related to the environment, stuff related to the object being rendered, as well as camera info

https://www.youtube.com/watch?v=akxevYYWd9g&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=33
*/

namespace ely
{
	// ShaderBuilder --------------------------------------------------------------------------------------------

	uint32_t ShaderBuilder::GetOpenGLShaderType(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::Vertex: return GL_VERTEX_SHADER;
			case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderType::TesselationControl: return GL_TESS_CONTROL_SHADER;
			case ShaderType::TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
			case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
			case ShaderType::Compute: return GL_COMPUTE_SHADER;
		};
		return 0;
	}

	ShaderBuilder& ShaderBuilder::Add(const ShaderType type, const std::string& filename)
	{
		const auto filepath = ShaderRepo::s_shader_asset_path + filename;
		//auto info = Compile(static_cast<uint32_t>(type), filepath);
		auto info = Compile(GetOpenGLShaderType(type), filepath);
		m_shader_list.push_back(info);
		return *this;
	}

	Ref<Shader> ShaderBuilder::Build(const std::string& name)
	{
		//auto shader = std::make_unique<Shader>();
		auto shader = CreateRef<Shader>();
		uint32_t program = glCreateProgram();

		shader->m_name = name;
		shader->m_program_id = program;
		bool success = true;

		bool compile_errors = false;
		for (auto& shader_info : m_shader_list)
		{
			if (shader_info.compile_success)
				glAttachShader(program, shader_info.id);
			else
				compile_errors = true;
		}

		if (compile_errors)
		{
			CORE_ERROR("Compilation errors in 1 or more shaders: {}", name);
			OutputShaderInfoLog();
			success = false;
		}

		glLinkProgram(program);
		int32_t result = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		if (result == GL_FALSE)
		{
			CORE_ERROR("Linking error for shader: {}", name);
			success = false;
		}

#ifdef DEBUG
		result = GL_FALSE;
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
		if (result == GL_FALSE)
		{
			CORE_ERROR("Validation error for shader: {}", name);
			success = false;
		}
#endif

		shader->m_build_success = success;

		for (auto& shader_info : m_shader_list)
			glDeleteShader(shader_info.id);

		m_shader_list.clear();
		return shader;
	}

	std::string ShaderBuilder::ReadSource(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		if (stream.fail())
		{
			CORE_ERROR("Could not open shader file: {}", filepath);
			return "";
		}
		std::string line;
		std::ostringstream ss;
		while (getline(stream, line))
			ss << line << "\n";
	
		return ss.str();
	}

	ShaderBuilder::ShaderInfo ShaderBuilder::Compile(uint32_t type, const std::string& filepath)
	{
		//NOTE
		/*
		Can do this :
			glBindAttribLocation (shader_programme, 0, "vertex_position");
			glBindAttribLocation (shader_programme, 1, "vertex_colour");

			Can also query the attribute location
		
			instead of setting location explicitly in the shader program (apparently you can't set location in shader in V3.2, which is the highest that can be run on a Mac

			See Anton pg 80, 217
		*/

		std::string code = ReadSource(filepath);
		ShaderInfo info;
		info.id = glCreateShader(type);
		info.filepath = filepath;
		info.type = type;
		const char* src = code.c_str();
		glShaderSource(info.id, 1, &src, nullptr);
		glCompileShader(info.id);
		int32_t result;
		glGetShaderiv(info.id, GL_COMPILE_STATUS, &result);
		info.compile_success = (result == GL_TRUE ? true : false);
		return info;
	}

	void ShaderBuilder::OutputShaderInfoLog()
	{
		for (auto& shader_info : m_shader_list)
		{
			int length;
			glGetShaderiv(shader_info.id, GL_INFO_LOG_LENGTH, &length);
			if (length > 0)
			{
				CORE_TRACE("   Shader info log: {}", shader_info.filepath);
				char* message = new char[length];
				glGetShaderInfoLog(shader_info.id, length, &length, message);
				CORE_TRACE("   filepath: {}, Compiled status: {}, id: {} message: {}", shader_info.filepath, shader_info.compile_success, shader_info.id, message);
			}
		}
	}


	// ShaderSource --------------------------------------------------------------------------------------------

	ShaderSource::ShaderSource(std::initializer_list<ShaderSourceFile> souce_files) :
		src{ souce_files}
	{
	}

	void ShaderSource::Add(ShaderSourceFile source_file)
	{
		src.push_back(source_file);
	}

	// Shader --------------------------------------------------------------------------------------------

	//TODO: - option to have all shaders in a single file (vs, fs etc)
	/*Ref<Shader> Shader::Create(const std::string& filepath, const std::string& name)
	{
		
	}*/

	Ref<Shader> Shader::Create(const ShaderSource& shader_source, const std::string& name)
	{
		ShaderBuilder builder;

		for (const auto& source_file : shader_source.src)
			builder.Add(source_file.first, source_file.second);
	
		return builder.Build(name);
	}

	void Shader::Bind()
	{
		glUseProgram(m_program_id);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::SetUniform1f(const std::string& name, float v0)
	{
		glUniform1f(GetUniformLocation(name), v0);
	}

	void Shader::SetUniform2f(const std::string& name, float v0, float v1)
	{
		glUniform2f(GetUniformLocation(name), v0, v1);
	}

	void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
	{
		glUniform3f(GetUniformLocation(name), v0, v1, v2);
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
	}

	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniform2f(const std::string& name, const glm::vec2& data)
	{
		glUniform2f(GetUniformLocation(name), data[0], data[1]);
	}

	void Shader::SetUniform3f(const std::string& name, const glm::vec3& data)
	{
		glUniform3f(GetUniformLocation(name), data[0], data[1], data[2]);
	}

	void Shader::SetUniform4f(const std::string& name, const glm::vec4& data)
	{
		glUniform4f(GetUniformLocation(name), data[0], data[1], data[2], data[3]);
	}

	void Shader::SetUniform1i(const std::string& name, int v0)
	{
		glUniform1i(GetUniformLocation(name), v0);
	}

	int32_t Shader::GetUniformLocation(const std::string& name)
	{
		if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
			return m_uniform_location_cache[name];
		int location = glGetUniformLocation(m_program_id, name.c_str());
		if (location == -1)
		{
			CORE_WARN("Uniform name: {} Doesn't exist.  Program ID: {} ", name, m_program_id);
		}
		m_uniform_location_cache[name] = location;
		return location;
	}

	void Shader::ReadUniforms()
	{
		int32_t params = -1;
		const int kMaxLength = 100;
		glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORMS, &params);
		for (int32_t i = 0; i < params; i++)
		{
			Shader::DataItem item;
			char name[kMaxLength];
			int actual_length = 0;
			int size = 0;
			GLenum type;
			glGetActiveUniform(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
			item.gl_type = type;
			for (int32_t element = 0; element < size; element++)
			{
				if (size == 1)
				{
					item.location = glGetUniformLocation(m_program_id, name);
					item.name = std::string(name);
				}
				else
				{
					std::string item_name = std::string{ name };
					item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
					item.location = glGetUniformLocation(m_program_id, item_name.c_str());
					item.name = item_name;
				}
				m_uniforms.push_back(item);
			}
		}
	}

	void Shader::ReadAttributes()
	{
		int32_t params = -1;
		const int kMaxLength = 100;
		glGetProgramiv(m_program_id, GL_ACTIVE_ATTRIBUTES, &params);
		for (int32_t i = 0; i < params; i++)
		{
			Shader::DataItem item;
			char name[kMaxLength];
			int actual_length = 0;
			int size = 0;
			GLenum type;
			glGetActiveAttrib(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
			item.gl_type = type;
			for (int32_t element = 0; element < size; element++)
			{
				if (size == 1)
				{
					item.location = glGetAttribLocation(m_program_id, name);
					item.name = std::string(name);
				}
				else
				{
					std::string item_name = std::string{ name };
					item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
					item.location = glGetAttribLocation(m_program_id, item_name.c_str());
					item.name = item_name;
				}
				m_attributes.push_back(item);
			}
		}
	}

	void Shader::OutputInfo()
	{
#ifndef DEBUG
		return;
#endif

		if (!m_build_success)
		{
			CORE_ERROR("   BUILD FAILED: {}", m_name);
			return;
		}
		
		ReadAttributes();
		ReadUniforms();

		int32_t params = -1;

		CORE_INFO("SHADER CREATED -------------------------------------");
		CORE_TRACE("   Program Name: {}", m_name);
		CORE_TRACE("   Program Id: {}", m_program_id);
		
		glGetProgramiv(m_program_id, GL_LINK_STATUS, &params);
		CORE_TRACE("   GL_LINK_STATUS {}", params);
		glGetProgramiv(m_program_id, GL_ATTACHED_SHADERS, &params);
		CORE_TRACE("   GL_ATTACHED_SHADERS {}", params);
	
		int length = 0;
		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetProgramInfoLog(m_program_id, length, &length, message);
		CORE_TRACE("   GL_INFO_LOG: {}", message);
			
		
		CORE_INFO("GL_ACTIVE_ATTRIBUTES {}", m_attributes.size());
		for (auto& item : m_attributes)
		{
			CORE_TRACE("   {}: {}, loc = {}", item.name, ShaderUtils::OpenGLTypeTypeToString(item.gl_type), item.location);
		}
		CORE_INFO("GL_ACTIVE_UNIFORMS {}", m_uniforms.size());
		for (auto& item : m_uniforms)
		{
			CORE_TRACE("   {}: {}, loc = {}", item.name, ShaderUtils::OpenGLTypeTypeToString(item.gl_type), item.location);
		}
	}

	// ShaderRepo --------------------------------------------------------------------------------------------

	std::unordered_map<std::string, Ref<Shader>> ShaderRepo::m_shader_repo{};
	std::string const ShaderRepo::s_shader_asset_path = std::string{ "../../assets/shaders/" };

	void ShaderRepo::Init()
	{
		ShaderRepo::LoadDefaultShaders();
	}

	Ref<Shader> ShaderRepo::Load(const ShaderSource& shader_source, const std::string& name)
	{
		//TODO: assert that shader doesn't already exist
		Ref<Shader> shader = Shader::Create(shader_source, name);
		m_shader_repo[name] = shader;
		return shader;
	}

	Ref<Shader> ShaderRepo::Get(const std::string& name)
	{
		//TODO: assert that shader exists
		return m_shader_repo[name];
	}

	bool ShaderRepo::Exists(const std::string& name)
	{
		return m_shader_repo.find(name) != m_shader_repo.end();
	}

	void ShaderRepo::LoadDefaultShaders()
	{
		ShaderSource shader_source =
		{
			{ShaderType::Vertex, "light_map_diff_spec_ub.vs"},
			{ShaderType::Fragment, "light_map_diff_spec.fs"}
		};
		ShaderRepo::Load(shader_source, "light_map_diff_spec_ub");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "light_map_diff_spec.vs"},
			{ShaderType::Fragment, "light_map_diff_spec.fs"}
		};
		ShaderRepo::Load(shader_source, "light_map_diff_spec");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "white_ub.vs"},
			{ShaderType::Fragment, "white.fs"}
		};
		ShaderRepo::Load(shader_source, "white_ub");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "white.vs"},
			{ShaderType::Fragment, "white.fs"}
		};
		ShaderRepo::Load(shader_source, "white");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "colored_basic_ub.vs"},
			{ShaderType::Fragment, "colored_basic.fs"}
		};
		ShaderRepo::Load(shader_source, "colored_basic_ub");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "colored_basic.vs"},
			{ShaderType::Fragment, "colored_basic.fs"}
		};
		ShaderRepo::Load(shader_source, "colored_basic");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "colored_diffuse_ub.vs"},
			{ShaderType::Fragment, "colored_diffuse.fs"}
		};
		ShaderRepo::Load(shader_source, "colored_diffuse_ub");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "colored_diffuse.vs"},
			{ShaderType::Fragment, "colored_diffuse.fs"}
		};
		ShaderRepo::Load(shader_source, "colored_diffuse");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "model_loading.vs"},
			{ShaderType::Fragment, "model_loading.fs"}
		};
		ShaderRepo::Load(shader_source, "model_loading");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "coords_ub.vs"},
			{ShaderType::Fragment, "coords.fs"}
		};
		ShaderRepo::Load(shader_source, "coords_ub");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "coords.vs"},
			{ShaderType::Fragment, "coords.fs"}
		};
		ShaderRepo::Load(shader_source, "coords");

		shader_source.Reset();
		shader_source =
		{
			{ShaderType::Vertex, "gamma.vs"},
			{ShaderType::Fragment, "gamma.fs"}
		};
		ShaderRepo::Load(shader_source, "gamma_correction");
	}

}