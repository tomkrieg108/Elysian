#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include <glad/glad.h>
#include "opengl_vertex_array.h"
#include "opengl_buffer_layout.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include "opengl_buffer.h"

namespace ely

{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_id);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		//TODO
		// NOTE this gets called when a Mesh local variable is returned from a function, which messing things up
		// use Release() instead - see below
		//CORE_TRACE("vertex array destructor called: ID={}", m_id);
		//glDeleteVertexArrays(1, &m_id);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_id);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::Release() const
	{
		glDeleteVertexArrays(1, &m_id);
	}

	void OpenGLVertexArray::AddVertexBuffer(const OpenGLVertexBuffer& vertex_buffer)
	{
		//TODO - is there a need to store the vertex buffer- only need the num verticies for the draw call
		glBindVertexArray(m_id);
		vertex_buffer.Bind();
		const auto& layout = vertex_buffer.GetLayout();
		for (const auto& element : layout.GetElements())
		{
			glEnableVertexAttribArray(m_buffer_index);
			if (ShaderUtils::OpenGLBaseTypeIsFloat(element.data_type) )
			{
				glVertexAttribPointer(m_buffer_index, 
					ShaderUtils::GetComponentCount(element.data_type),
					ShaderUtils::ShaderDataTypeToOpenGLBaseType(element.data_type),
					element.normalized ? GL_TRUE : GL_FALSE, 
					layout.GetStride(), 
					(const void*)(element.offset));
			}
			else
			{
				glVertexAttribIPointer(m_buffer_index,
					ShaderUtils::GetComponentCount(element.data_type),
					ShaderUtils::ShaderDataTypeToOpenGLBaseType(element.data_type),
					layout.GetStride(),
					(const void*)(element.offset));
			}
			++m_buffer_index;
		}
		m_vertex_buffers.push_back(vertex_buffer);
		glBindVertexArray(0);
	}

}