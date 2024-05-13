#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/camera/camera.h"
#include "elysian/light/directional_light.h"
#include "elysian/model/mesh.h"
#include "elysian/scene/scene.h"
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
		static void DrawMesh(const Mesh& mesh, const Shader& shader);

		static void BeginScene(const PerspectiveCamera& camera, const OpenGLFramebuffer& frame_buffer);
		static void EndScene();

		static void BeginScene(const Scene& scene, const Camera& camera, const DirectionalLight& directional_light, 
			const OpenGLFramebuffer& render_target);
		static void BeginScene(const Scene& scene, const Camera& camera, const DirectionalLight& directional_light);
		static void Draw(const mesh_v2::Mesh& mesh, const material_v2::Material& material);
		
	private:
		static uint32_t GetOpenGLDrawMode(DrawMode draw_mode);

		/*
		TODO - render stats - number of draw calls, uniform uploads, total vertices
		*/
	};
}



