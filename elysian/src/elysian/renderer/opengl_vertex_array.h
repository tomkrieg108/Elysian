#pragma once

#include "opengl_buffer.h"

namespace ely
{
	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;
		void Release();

		void AddVertexBuffer(const OpenGLVertexBuffer& vertex_buffer);
		void SetIndexBuffer(const OpenGLIndexBuffer& index_buffer);

		const std::vector<OpenGLVertexBuffer>& GetVertexBuffers() const { return m_vertex_buffers; }
		const OpenGLIndexBuffer& GetIndexBuffer() const { return m_index_buffer; }
		const uint32_t GetId() const { return m_id; }

	private:
		uint32_t m_id = 0;
		uint32_t m_buffer_index = 0;
		std::vector<OpenGLVertexBuffer> m_vertex_buffers; //TODO - need multiple VB's?
		OpenGLIndexBuffer m_index_buffer;
	};
}