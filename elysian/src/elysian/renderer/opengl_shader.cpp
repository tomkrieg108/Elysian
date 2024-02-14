#include "pch.h"
#include <GL/glew.h>
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_shader.h"


//TODO
/*
Save the uniform locations for each uniform straight after compiling the shader rather than caching them

material (according to cherno) would include a shader plus all the data that the shader needs (except lighting and camera data)

Material* material = new Material(shader)
queries all the data that the shader needs
renderer will set the uniforms related to transforms, projections, light properties etc.
material sets the uniforms related to the surface properties
*/

namespace ely
{
	std::string const ShaderBuilder::s_shader_path = std::string{ "../../assets/shaders/" };

	ShaderBuilder& ShaderBuilder::BuildShader(const ShaderType type, const std::string& filename)
	{
		const auto filepath = ShaderBuilder::s_shader_path + filename;
		auto info = Compile(static_cast<uint32_t>(type), filepath);
		m_shader_list.push_back(info);
		return *this;
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
		//TODO
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


	std::unique_ptr<Shader> ShaderBuilder::BuildProgram(const std::string& name)
	{
		auto shader = std::make_unique<Shader>();
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
		//validation check in debug mode only
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
//-------------------------------------------------------------------------------------------------------------------------------

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

	void Shader::SetUniform2f(const std::string& name, glm::vec2& data)
	{
		glUniform2f(GetUniformLocation(name), data[0], data[1]);
	}

	void Shader::SetUniform3f(const std::string& name, glm::vec3& data)
	{
		glUniform3f(GetUniformLocation(name), data[0], data[1], data[2]);
	}

	void Shader::SetUniform4f(const std::string& name, glm::vec4& data)
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
			CORE_TRACE("   {}: {}, loc = {}", item.name, GLTypeToString(item.gl_type), item.location);
		}
		CORE_INFO("GL_ACTIVE_UNIFORMS {}", m_uniforms.size());
		for (auto& item : m_uniforms)
		{
			CORE_TRACE("   {}: {}, loc = {}", item.name, GLTypeToString(item.gl_type), item.location);
		}
	}

	const std::string GLTypeToString(GLenum type)
	{
		switch (type)
		{
			case GL_BOOL: return std::string("GL_BOOL");
			case GL_INT: return std::string("GL_INT");
			case GL_FLOAT: return std::string("GL_FLOAT");
			case GL_FLOAT_VEC2: return std::string("GL_FLOAT_VEC2");
			case GL_FLOAT_VEC3: return std::string("GL_FLOAT_VEC3"); 
			case GL_FLOAT_VEC4: return std::string("GL_FLOAT_VEC4");
			case GL_FLOAT_MAT2: return std::string("GL_FLOAT_MAT2");
			case GL_FLOAT_MAT3: return std::string("GL_FLOAT_MAT3");
			case GL_FLOAT_MAT4: return std::string("GL_FLOAT_MAT4");
			case GL_SAMPLER_2D: return std::string("GL_SAMPLER_2D");
			case GL_SAMPLER_3D: return std::string("GL_SAMPLER_3D");
			case GL_SAMPLER_CUBE: return std::string("GL_SAMPLER_CUBE");
			case GL_SAMPLER_2D_SHADOW: return std::string("GL_SAMPLER_2D_SHADOW");
			default: return std::string("OTHER");
		}
	}

	void DisplayUniformValue(uint32_t program, int location, GLenum type)
	{
		if (type == GL_INT)
		{
			int val = 0;
			glGetUniformiv(program, location, &val);
			CORE_INFO("Val: {}: ", val);
		}
		if (type == GL_SAMPLER_2D)
		{
			int val = 0;
			glGetUniformiv(program, location, &val);
			CORE_INFO("Val: {}: ", val);
		}
		if (type == GL_FLOAT)
		{
			float val = 0;
			glGetUniformfv(program, location, &val);
			CORE_INFO("Val: {}: ", val);
		}
	}

}