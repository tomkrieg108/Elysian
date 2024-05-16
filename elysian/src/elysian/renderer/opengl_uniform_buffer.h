#pragma once

namespace ely
{
	class OpenGLUniformBuffer
	{
	public:
		OpenGLUniformBuffer() = default; //needed for declaring m_uniform_buffers container in opengl_shader.cpp
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		~OpenGLUniformBuffer();

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) const;

	private:
		uint32_t m_id = 0;
	};
}