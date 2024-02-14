#pragma once

namespace ely
{
	class OpenGLVertexBuffer;

	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const OpenGLVertexBuffer& vertex_buffer);
		const std::vector<OpenGLVertexBuffer>& GetVertexBuffers() const { return m_vertex_buffers; }

	private:
		uint32_t m_id = 0;
		uint32_t m_buffer_index = 0;
		std::vector<OpenGLVertexBuffer> m_vertex_buffers;
	};


}