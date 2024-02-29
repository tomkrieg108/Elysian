#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "material.h"

namespace ely
{
	/*
	NOTE
	Mesh should contain Vertex data (defining geometry, Material data (defining surface properties)
	And maybe a shader
	*/

	class Mesh
	{
	public:
		Mesh(const OpenGLVertexBuffer& vertex_buffer, const BufferLayout& layout, Ref<Material> material) :
			m_material{ material }
		{
			m_vao.AddVertexBuffer(vertex_buffer);
		}

		void SetMaterial(Ref<Material> material) { m_material = material; }
		Material& GetMaterial() { return *m_material; } //ret val need to be non-const to get the range based for loops to compile!
		const OpenGLVertexArray& GetVertexArray() { return m_vao; }

	private:
		Ref<Material> m_material;
		OpenGLVertexArray m_vao;
	};
	
}