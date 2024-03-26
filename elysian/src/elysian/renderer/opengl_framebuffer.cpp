#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "elysian/kernal/log.h"
#include "opengl_framebuffer.h"

namespace ely
{
	OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height, const OpenGLTexture2D::Params& texture_params) 
	{
    Reset(width, height, texture_params);
	}

  void OpenGLFramebuffer::Reset(uint32_t width, uint32_t height, const OpenGLTexture2D::Params& texture_params)
  {
    m_width = width;
    m_height = height;
    m_texture_params = texture_params;

    if (m_id != 0)
    {
      glDeleteFramebuffers(1, &m_id);
      glDeleteTextures(1, &m_colour_attachment_id);
      glDeleteRenderbuffers(1, &m_depth_attachment_id);
    }
    
    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // create a color attachment texture
    glGenTextures(1, &m_colour_attachment_id);
    glBindTexture(GL_TEXTURE_2D, m_colour_attachment_id);
    uint32_t internal_format = m_texture_params.enable_srgb ? GL_SRGB8 : GL_SRGB;
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_MIN_FILTER, m_texture_params.MIN_FILTER);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_MAG_FILTER, m_texture_params.MAG_FILTER);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_WRAP_S, m_texture_params.WRAP_S);
    glTextureParameteri(m_colour_attachment_id, GL_TEXTURE_WRAP_T, m_texture_params.WRAP_T);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colour_attachment_id, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &m_depth_attachment_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_attachment_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height); // use a single renderbuffer object for both a depth AND stencil buffer.
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

  void OpenGLFramebuffer::Bind() const
  {
    //NOTE:
    //glBindFramebuffer(GL_FRAMEBUFFER, 0); //renders to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    glViewport(0, 0, m_width, m_height);
  }

  void OpenGLFramebuffer::Unbind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}