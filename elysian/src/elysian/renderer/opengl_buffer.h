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

	//-----------------------------------------------------------

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetIndexCount() const { return m_index_count; }

	private:
		uint32_t m_id = 0;
		uint32_t m_index_count = 0;
	};

}