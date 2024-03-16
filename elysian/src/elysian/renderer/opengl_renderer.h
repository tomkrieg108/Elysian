#pragma once
#include "elysian/kernal/base.h"
#include "elysian/model/mesh.h"
#include "opengl_shader.h"
#include "opengl_shader.h"
#include "opengl_draw_mode.h"

namespace ely
{
	

	class OpenGLRenderer
	{
	public:
		static void Init();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetViewport(uint32_t width, uint32_t height);
		static void SetClearColor(const glm::vec4& color);
		static void ClearBuffers();
		static void SetLineWidth(float width);
	
		static void DrawMesh(const Ref<Mesh>& mesh, const Ref<Shader>& shader, DrawMode draw_mode = DrawMode::Triangles);
		//static void DrawMesh(const Mesh& mesh, const Ref<Shader>& shader, DrawMode draw_mode = DrawMode::Triangles);
		
	private:
		static uint32_t GetOpenGLDrawMode(DrawMode draw_mode);

		/*
		TODO - render stats - number of draw calls, uniform uploads, total vertices
		*/
	};
}


/*

//renderer::BeginScene()  sets up camera info and environment info for the shader
// for a specific object in the scene - need to provide shader with material and transform
Renderer::BeginScene(camera, lights, env)

Renderer::Submit() - input could be a model, mesh, vertex buffer ... (overload)
Renderer::Submit()
Renderer::Submit()
Renderer::Submit()
//these all get quesed up - not yet actually drawn

Renderer::EndScene()

Renderer::Flush() (usually done on seperate thread)

mesh will include a material(which will correspond to shader data)
model will include 1 or more meshes plus possibly other stuff


*/


