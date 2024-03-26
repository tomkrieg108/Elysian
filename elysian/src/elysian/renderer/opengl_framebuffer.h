#pragma once

#include "elysian/renderer/opengl_texture_2d.h"

namespace ely
{
	class OpenGLFramebuffer
	{
	public:
		OpenGLFramebuffer(uint32_t width, uint32_t height, 
			const OpenGLTexture2D::Params& params = { GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,true,false,false });
		~OpenGLFramebuffer();

		void Reset(uint32_t width, uint32_t height, const OpenGLTexture2D::Params& params ={ GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT,true,false,false });

		uint32_t GetColourAttachmentID() const {
			return m_colour_attachment_id;
		}

		uint32_t GetDepthAttachmentID() const {
			return m_depth_attachment_id;
		}

		void Bind() const;
		void Unbind() const;

		bool IsComplete() const { return m_is_complete; }

	private:
		uint32_t m_id = 0;
		uint32_t m_colour_attachment_id = 0;
		uint32_t m_depth_attachment_id = 0;
		
		//TODO - setup a params struct to pass into a framebuffer (similar to texture2d)
		uint32_t m_width = 1200;
		uint32_t m_height = 720;
		OpenGLTexture2D::Params m_texture_params;

		bool m_is_complete = false;
	};
}