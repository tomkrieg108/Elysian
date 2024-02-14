#pragma once

#include "opengl_buffer_layout.h"

namespace ely
{
	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer(void* const data, uint32_t size, const BufferLayout& layout);
		~OpenGLVertexBuffer();
		void Bind() const;
		void Unbind() const;
		const BufferLayout& GetLayout() const { return m_layout; }
		uint32_t GetVertexCount() const { return m_vertex_count;  }

	private:
		uint32_t m_id = 0;
		BufferLayout m_layout;
		uint32_t m_vertex_count = 0;
	};

}