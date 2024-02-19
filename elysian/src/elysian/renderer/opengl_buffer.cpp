#include "pch.h"
#include <glad/glad.h>
#include "opengl_buffer_layout.h"
#include "opengl_buffer.h"

namespace ely
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(void* const data, uint32_t size, const BufferLayout& layout) :
		m_layout{ layout }
	{
		//glCreateBuffers(1, &m_id); //V4.5
		glGenBuffers(1, &m_id); //V2.0
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		m_vertex_count = size / layout.getStride();
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}