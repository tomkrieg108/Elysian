#include "pch.h"
#include <GL/glew.h>
#include "elysian/kernal/log.h"
#include "opengl_framebuffer.h"

namespace ely
{
	OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height, const OpenGLTexture2D::Params& params)
	{
    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // create a color attachment texture
    glGenTextures(1, &m_colour_attachment_id);
    glBindTexture(GL_TEXTURE_2D, m_colour_attachment_id);
    uint32_t internal_format = params.enable_srgb ? GL_SRGB8 : GL_SRGB;
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   /* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_MIN_FILTER, params.MIN_FILTER); 
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_MAG_FILTER, params.MAG_FILTER);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_WRAP_S, params.WRAP_S);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_WRAP_T, params.WRAP_T);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colour_attachment_id, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    uint32_t rbo;
    glGenRenderbuffers(1, &m_depth_attachment_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_attachment_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_attachment_id); // now actually attach it

    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      CORE_ERROR("Framebuffer is not complete");
    else
      m_is_complete = true;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
    glDeleteFramebuffers(1, &m_id);
    glDeleteTextures(1, &m_colour_attachment_id);
    glDeleteRenderbuffers(1, &m_depth_attachment_id);
	}

  void OpenGLFramebuffer::Bind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
  }

  void OpenGLFramebuffer::Unbind()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}