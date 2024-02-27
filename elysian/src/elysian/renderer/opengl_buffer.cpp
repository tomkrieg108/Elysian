#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "opengl_buffer_layout.h"
#include "opengl_buffer.h"

namespace ely
{
	//----------------------------------------------------------------------------------------------
	// Vertex Buffer
	//----------------------------------------------------------------------------------------------

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* const data, uint32_t size, const BufferLayout& layout) :
		m_layout{ layout }
	{
		glCreateBuffers(1, &m_id); //V4.5
		//glGenBuffers(1, &m_id); //V2.0
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		m_vertex_count = size / layout.GetStride();
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

	//----------------------------------------------------------------------------------------------
	// Index Buffer
	//----------------------------------------------------------------------------------------------

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
	{
		glCreateBuffers(1, &m_id);

		//From Hazel:
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}