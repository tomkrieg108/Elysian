#include "pch.h"
#include <glad/glad.h>
#include "opengl_vertex_array.h"
#include "opengl_buffer_layout.h"
#include "opengl_buffer.h"

namespace ely
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_id);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_id);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const OpenGLVertexBuffer& vertex_buffer)
	{
		glBindVertexArray(m_id);
		vertex_buffer.Bind();
		const auto& layout = vertex_buffer.GetLayout();
		uint32_t offset = 0;
		for (auto& element : layout.getElements())
		{
			glEnableVertexAttribArray(m_buffer_index);
			if (element.type == GL_FLOAT)
			{
				glVertexAttribPointer(m_buffer_index, element.count, element.type,
					element.normalised ? GL_TRUE : GL_FALSE, layout.getStride(), (const void*)(offset));
			}
			else
			{
				glVertexAttribIPointer(m_buffer_index, element.count, element.type, layout.getStride(), (const void*)(offset));
			}
			offset += element.NumBytes();
			++m_buffer_index;
		}
		m_vertex_buffers.push_back(vertex_buffer);
		glBindVertexArray(0);
	}

}