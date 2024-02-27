#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "opengl_renderer.h"

namespace ely
{
	
	void OpenGLRenderer::Init()
	{
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // AOGL p20
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		 glViewport(x, y, width, height);
	}

	void OpenGLRenderer::SetViewport(uint32_t width, uint32_t height)
	{
		glViewport(0,0,width, height);
	}

	void OpenGLRenderer::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderer::ClearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderer::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

}

