#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/renderer/opengl_uniform_buffer.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/camera/camera.h"
#include "elysian/light/directional_light.h"
#include "elysian/model/mesh.h"
#include "opengl_shader.h"
#include "opengl_draw_mode.h"

namespace ely
{
	class OpenGLRenderer
	{
	public:
		
		//Shader Uniform block definitions
		//TODO - maybe put in shader class or shader untils?
		struct CameraBlock
		{
			alignas(16) glm::mat4 proj;
			alignas(16) glm::mat4 view;
			alignas(16) glm::vec3 eye_pos;
		};

		struct LightBlock
		{
			alignas(16) glm::vec3 dir;
			alignas(16) glm::vec3 color;
			alignas(16) glm::vec3 ambient_color;
		};


	public:
			static void Init();
			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			static void SetClearColor(const glm::vec4& color);
			static void ClearBuffers();
			static void SetLineWidth(float width);

			static void Draw(const Mesh& mesh, const glm::mat4& transform,  Shader& shader);

			static void Draw_V2(const mesh_v2::Mesh& mesh, const material_v2::Material& material, 
				const glm::mat4& transform, Shader& shader);
			 
			static void Begin(const Camera& camera, const DirectionalLight& dir_light,
				const glm::mat4& camera_transform, const glm::mat4& light_transform);

			static void End();

		
	private:
		 static uint32_t GetOpenGLDrawMode(DrawMode draw_mode);

		/*
		TODO - render stats - number of draw calls, uniform uploads, total vertices
		*/
	};
}



