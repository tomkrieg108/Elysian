#include "pch.h"
#include <glad/glad.h>
#include "opengl_uniform_buffer.h"

namespace ely
{

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		Init(size, binding);
	}

	void OpenGLUniformBuffer::Init(uint32_t size, uint32_t binding)
	{
		//V4.5 way (as per Cherno)
		glCreateBuffers(1, &m_id); //rather then glGenBuffers()
		glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_id, offset, size, data);
	}

	void OpenGLUniformBuffer::Bind()
	{
	}

	void OpenGLUniformBuffer::Unbind()
	{
	}

}