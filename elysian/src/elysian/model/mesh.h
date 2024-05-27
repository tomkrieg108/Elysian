#pragma once
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_draw_mode.h"
#include "material.h"

namespace ely
{
	/*
	NOTE
	Mesh should contain Vertex data (defining geometry, Material data (defining surface properties)
	And maybe a shader

	//TODO might want material to be seperated from mesh?  But then mesh will just be a vertex array!
	*/

	inline namespace mesh_v1 {
		class Mesh
		{
		public:

			Mesh() = default;
			Mesh(const OpenGLVertexBuffer& vertex_buffer, const Material& material, DrawMode draw_mode = DrawMode::Triangles);
			~Mesh() {}

			void SetMaterial(const Material& material) { m_material = material; }
			const Material& GetMaterial() const { return m_material; }

			void SetVertexArray(const OpenGLVertexArray& vao) { m_vao = vao; }
			const OpenGLVertexArray& GetVertexArray() const { return m_vao; }

			void SetDrawMode(DrawMode draw_mode) { m_draw_mode = draw_mode; }
			const DrawMode GetDrawMode() const { return m_draw_mode; }

			void UploadMaterialToShader(Shader& shader) const { m_material.UploadDataToShader(shader); }

		private:
			Material m_material;
			OpenGLVertexArray m_vao;
			DrawMode m_draw_mode = DrawMode::Triangles; //Unity calls this "topology"
		};
	}

	namespace mesh_v2 {


		class Mesh
		{
		public:

			Mesh() = default;
			Mesh(const Mesh& other) = default;

			Mesh(const OpenGLVertexBuffer& vertex_buffer);
			Mesh(const OpenGLVertexBuffer& vertex_buffer, const OpenGLIndexBuffer& index_buffer);
			Mesh(void* const data, uint32_t size, const BufferLayout& layout);
			Mesh(void* const data, uint32_t size, const BufferLayout& layout, uint32_t* indices, uint32_t count);

			const OpenGLVertexArray& GetVertexArray() const { return m_vao; }

			void SetDrawMode(DrawMode draw_mode) { m_draw_mode = draw_mode; }
			DrawMode GetDrawMode() const { return m_draw_mode; }
			
		private:
			OpenGLVertexArray m_vao;
			DrawMode m_draw_mode = DrawMode::Triangles;
		};

	}

}