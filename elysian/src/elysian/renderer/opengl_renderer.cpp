#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/model/mesh.h"
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

	void OpenGLRenderer::DrawMesh(Ref<Mesh> mesh, Ref<Shader> shader, DrawMode draw_mode)
	{
		auto& material = mesh->GetMaterial();
		const auto& vao = mesh->GetVertexArray();

		shader->Bind();
		material.UploadDataToShader(shader);
		vao.Bind();

		//TODO - handle multiple vbos
		auto& vbo = vao.GetVertexBuffers()[0];
		glDrawArrays(GetOpenGLDrawMode(draw_mode), 0, vbo.GetVertexCount());
	}


	uint32_t OpenGLRenderer::GetOpenGLDrawMode(DrawMode draw_mode)
	{
		switch (draw_mode)
		{
		case DrawMode::Triangles: return GL_TRIANGLES;
		case DrawMode::Lines: return GL_LINES;
		}

		//TODO assert unknown draw mode
		return GL_TRIANGLES;
	}

}

