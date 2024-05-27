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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Anton loc 607
		glEnable(GL_LINE_SMOOTH);

		//TODO - this causes display problems!
		//glEnable(GL_BLEND); 
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Anton loc 808
		//TODO the winding order on the cube triangles is not consistent, so the following causes some of the cube faces to not be drawn
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CW);	//or GL_CCW 
	}

	void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		 glViewport(x, y, width, height);
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

	void OpenGLRenderer::Draw(const Mesh& mesh, const glm::mat4& transform, Shader& shader)
	{
		auto& vao = mesh.GetVertexArray();
		shader.Bind();
		vao.Bind();

		mesh.UploadMaterialToShader(shader); //TODO shoud be done in renderer

		shader.SetUniformMat4f("u_model",transform);
		//TODO - handle multiple vbo's ?
		auto& vbo = vao.GetVertexBuffers()[0];
		glDrawArrays(GetOpenGLDrawMode(mesh.GetDrawMode()), 0, vbo.GetVertexCount());
		vao.Unbind();
		shader.Unbind();
	} 

#if 1
	void OpenGLRenderer::Draw_V2(const mesh_v2::Mesh& mesh, const material_v2::Material& material, 
		const glm::mat4& transform, Shader& shader)
	{
		auto& vao = mesh.GetVertexArray();
		shader.Bind();
		vao.Bind();
		material.UploadDataToShader();
		shader.SetUniformMat4f("u_model", transform);
		//TODO - handle multiple vbo's ?
		auto& vbo = vao.GetVertexBuffers()[0];
		glDrawArrays(GetOpenGLDrawMode(mesh.GetDrawMode()), 0, vbo.GetVertexCount());
		vao.Unbind();
		shader.Unbind();
	}
#endif

	void OpenGLRenderer::Begin(const Camera& camera, const DirectionalLight& light,
		const glm::mat4& camera_transform, const glm::mat4& light_transform)
	{
		//Upload camera uniform buffer to GPU
		const auto& camera_ub_opt = ShaderRepo::GetUniformBuffer("ub_camera");
		if (camera_ub_opt)
		{
			glm::mat4 view_mat = camera.GetViewMatrix(camera_transform);
			glm::mat4 proj_mat = camera.GetProjMatrix();
			glm::vec3 eye_pos = glm::vec3{ camera_transform[3] };
			CameraBlock camera_block{ proj_mat, view_mat, eye_pos };
			const OpenGLUniformBuffer& camera_ub = camera_ub_opt.value();
			camera_ub.SetData(reinterpret_cast<const void*>(&camera_block), sizeof(CameraBlock));
		}

		//Upload directional light unitorm buffer to GPU
		const auto& light_ub_opt = ShaderRepo::GetUniformBuffer("ub_directional_light");
		if (light_ub_opt)
		{
			LightBlock light_block{ light.direction, light.color, light.ambient_color };
			const OpenGLUniformBuffer& light_ub = light_ub_opt.value();
			light_ub.SetData(reinterpret_cast<const void*>(&light_block), sizeof(LightBlock));
		}
	}

	void OpenGLRenderer::End()
	{
	}

	//TODO - remove this - draw mode should be 'mesh topology' - property of the mesh
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

