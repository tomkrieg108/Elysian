#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "opengl_uniform_buffer.h"
#include <glad/glad.h>

namespace ely
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_id); //V4.5
		glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		CORE_ERROR("OpenGLUniformBuffer destructor called: ID={}", m_id);
		//glDeleteBuffers(1, &m_id);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) const
	{
		glNamedBufferSubData(m_id, offset, size, data); //V4.5
	}
}