#pragma once

namespace ely
{
	//Don't want the client app using OpenGL data type enum vals, so make a custom enum (from Hazel again!)
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Sampler2D
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
		static std::string  OpenGLTypeTypeToString(uint32_t type); //param type is GLenum which is an unsigned int 
		static void DisplayUniformValue(uint32_t program, int location, uint32_t type);
	};
 }