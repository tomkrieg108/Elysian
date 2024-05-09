#include "pch.h"
#include "elysian/kernal/base.h"
#include "mesh.h"

namespace ely
{
	inline namespace mesh_v1 {

		Mesh::Mesh(const OpenGLVertexBuffer& vertex_buffer, const Material& material, DrawMode draw_mode) :
			m_material{ material }, m_draw_mode{ draw_mode }
		{
			m_vao.AddVertexBuffer(vertex_buffer);
		}

	}

	namespace mesh_v2 {

		Mesh::Mesh(const OpenGLVertexBuffer& vertex_buffer)
		{
			m_vao.AddVertexBuffer(vertex_buffer);
		}

		Mesh::Mesh(const OpenGLVertexBuffer& vertex_buffer, const OpenGLIndexBuffer& index_buffer)
		{
			m_vao.AddVertexBuffer(vertex_buffer);
			m_vao.SetIndexBuffer(index_buffer);
		}

		Mesh::Mesh(void* const data, uint32_t size, const BufferLayout& layout)
		{
			m_vao.AddVertexBuffer({ data, size, layout });
		}

		Mesh::Mesh(void* const data, uint32_t size, const BufferLayout& layout, uint32_t* indices, uint32_t count)
		{
			m_vao.AddVertexBuffer({ data, size, layout });
			m_vao.SetIndexBuffer({ indices,count });
		}

	}
}
