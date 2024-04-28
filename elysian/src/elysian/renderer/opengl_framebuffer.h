#pragma once

#include "elysian/renderer/opengl_texture_2d.h"

namespace ely
{
	//TODO set this up with input parameters / spec (similar to texture2D)
	class OpenGLFramebuffer
	{
	public:

		struct FrameBufferParams
		{
			uint32_t width{ 1200 };
			uint32_t height{ 720 };
			OpenGLTexture2D::Params tex_params{ GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,true,false,false }; //TODO  compile error for c++20 - ok in C++ 17
		};

	public:
		
		OpenGLFramebuffer(FrameBufferParams params);
		OpenGLFramebuffer(uint32_t width, uint32_t height );
		~OpenGLFramebuffer();

		void Reset(FrameBufferParams params);
		void Reset(uint32_t width, uint32_t height);

		uint32_t GetColourAttachmentID() const {return m_colour_attachment_id;}
		uint32_t GetDepthAttachmentID() const { return m_depth_attachment_id; }
		
		uint32_t GetWidth() const { return m_parameters.width; }
		uint32_t GetHeight() const { return m_parameters.height; }
		float GetAspectRatio() const { return (float(m_parameters.width) / (float)(m_parameters.height)); }

		void Bind() const;
		void Unbind() const;
		bool IsComplete() const { return m_is_complete; }

	private:
		uint32_t m_id = 0;
		uint32_t m_colour_attachment_id = 0;
		uint32_t m_depth_attachment_id = 0;

		FrameBufferParams m_parameters;
		bool m_is_complete = false;
	};
}