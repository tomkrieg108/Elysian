#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "opengl_shader_utils.h"

namespace ely
{
	 uint32_t ShaderUtils::ShaderDataTypeSize(ShaderDataType type)
	 {
			switch (type)
			{
				case ShaderDataType::Float:    return 4;
				case ShaderDataType::Float2:   return 4 * 2;
				case ShaderDataType::Float3:   return 4 * 3;
				case ShaderDataType::Float4:   return 4 * 4;
				case ShaderDataType::Mat3:     return 4 * 3 * 3;
				case ShaderDataType::Mat4:     return 4 * 4 * 4;
				case ShaderDataType::Int:      return 4;
				case ShaderDataType::Int2:     return 4 * 2;
				case ShaderDataType::Int3:     return 4 * 3;
				case ShaderDataType::Int4:     return 4 * 4;
				case ShaderDataType::Bool:     return 1;
			}
			return 0;
		}

		uint32_t ShaderUtils::GetComponentCount(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Mat3:    return 3; // 3* float3
				case ShaderDataType::Mat4:    return 4; // 4* float4
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Bool:    return 1;
			}
			return 0;
		}

		uint32_t ShaderUtils::ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
					return GL_FLOAT;
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Sampler2D:
					return GL_INT;
				case ShaderDataType::Bool:
					return GL_BOOL;
			}

			//HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}

		bool ShaderUtils::OpenGLBaseTypeIsFloat(ShaderDataType type)
		{
			return ShaderDataTypeToOpenGLBaseType(type) == GL_FLOAT;
		}

		bool ShaderUtils::OpenGLBaseTypeIsInt(ShaderDataType type)
		{
			return ShaderDataTypeToOpenGLBaseType(type) == GL_INT;
		}

		bool ShaderUtils::OpenGLBaseTypeIsBool(ShaderDataType type)
		{
			return ShaderDataTypeToOpenGLBaseType(type) == GL_BOOL;
		}

		std::string ShaderUtils::ShaderDataTypeToString(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Bool: return std::string("GL_BOOL");
				case ShaderDataType::Int: return std::string("GL_INT");
				case ShaderDataType::Int2: return std::string("GL_INT_VEC2");
				case ShaderDataType::Int3: return std::string("GL_INT_VEC3");
				case ShaderDataType::Int4: return std::string("GL_INT_VEC4");
				case ShaderDataType::Float: return std::string("GL_FLOAT");
				case ShaderDataType::Float2: return std::string("GL_FLOAT_VEC2");
				case ShaderDataType::Float3: return std::string("GL_FLOAT_VEC3");
				case ShaderDataType::Float4: return std::string("GL_FLOAT_VEC4");
				case ShaderDataType::Mat2: return std::string("GL_FLOAT_MAT2");
				case ShaderDataType::Mat3: return std::string("GL_FLOAT_MAT3");
				case ShaderDataType::Mat4: return std::string("GL_FLOAT_MAT4");
				case ShaderDataType::Sampler2D: return std::string("GL_SAMPLER_2D");
				case ShaderDataType::Sampler3D: return std::string("GL_SAMPLER_3D");
				case ShaderDataType::SamplerCube: return std::string("GL_SAMPLER_CUBE");
				case ShaderDataType::Sample2DShadow: return std::string("GL_SAMPLER_2D_SHADOW");
				default: return std::string("OTHER");
			}
		}
	
		ShaderDataType ShaderUtils::OpenGLTypeToShaderDataType(uint32_t gl_type)
		{
			switch (gl_type)
			{
				case GL_BOOL: return ShaderDataType::Bool;
				case GL_INT: return ShaderDataType::Int;
				case GL_INT_VEC2: return ShaderDataType::Int2;
				case GL_INT_VEC3: return ShaderDataType::Int3;
				case GL_INT_VEC4: return ShaderDataType::Int4;
				case GL_FLOAT: return ShaderDataType::Float;
				case GL_FLOAT_VEC2: return ShaderDataType::Float2;
				case GL_FLOAT_VEC3: return ShaderDataType::Float3;
				case GL_FLOAT_VEC4: return ShaderDataType::Float4;
				case GL_FLOAT_MAT2: return ShaderDataType::Mat2;
				case GL_FLOAT_MAT3: return ShaderDataType::Mat3;
				case GL_FLOAT_MAT4: return ShaderDataType::Mat4;
				case GL_SAMPLER_2D: return ShaderDataType::Sampler2D;
				case GL_SAMPLER_3D: return ShaderDataType::Sampler3D;
				case GL_SAMPLER_CUBE: return ShaderDataType::SamplerCube;
				case GL_SAMPLER_2D_SHADOW: return ShaderDataType::Sample2DShadow;
				default: return ShaderDataType::None;
			}
		}

		void ShaderUtils::DisplayUniformValue(uint32_t program, int location, uint32_t gl_type)
		{
			if (gl_type == GL_INT)
			{
				int val = 0;
				glGetUniformiv(program, location, &val);
				std::cout << " Val = " << val;
			}
			if (gl_type == GL_SAMPLER_2D)
			{
				int val = 0;
				glGetUniformiv(program, location, &val);
				std::cout << " Val = " << val;
			}
			if (gl_type == GL_FLOAT)
			{
				float val = 0;
				glGetUniformfv(program, location, &val);
				std::cout << " Val = " << val;
			}
		}
}