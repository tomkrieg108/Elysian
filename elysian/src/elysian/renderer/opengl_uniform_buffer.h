#pragma once

namespace ely
{
	class OpenGLUniformBuffer
	{
	public:
		OpenGLUniformBuffer() = default;
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		~OpenGLUniformBuffer();

		void Init(uint32_t size, uint32_t binding);
		void SetData(const void* data, uint32_t size, uint32_t offset = 0);

		void Bind();
		void Unbind();
	
	private:
		uint32_t m_id = 0;
	};
}