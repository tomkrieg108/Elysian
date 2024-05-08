#pragma once

namespace ely
{
	//Don't want the client app using OpenGL data type enum vals, so make a custom enum (from Hazel again!)
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat2, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Sampler2D, Sampler3D, SamplerCube, Sample2DShadow
	};

	class ShaderUtils
	{
	public:
		static uint32_t ShaderDataTypeSize(ShaderDataType type);
		static uint32_t GetComponentCount(ShaderDataType type);
		static uint32_t ShaderDataTypeToOpenGLBaseType(ShaderDataType type); //return type is GLenum which is an unsigned int 
		static bool OpenGLBaseTypeIsFloat(ShaderDataType type);
		static bool OpenGLBaseTypeIsInt(ShaderDataType type);
		static bool OpenGLBaseTypeIsBool(ShaderDataType type);
		
		static void DisplayUniformValue(uint32_t program, int location, uint32_t type);

		static std::string  ShaderDataTypeToString(ShaderDataType type); //param type is GLenum which is an unsigned int 
		static ShaderDataType OpenGLTypeToShaderDataType(uint32_t gl_type); //param type is GLenum which is an unsigned int 
	};
 }