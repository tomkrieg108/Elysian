#pragma once
#include "elysian/kernal/base.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "material.h"

namespace ely
{
	/*
	Mesh should contain Vertex data (defining geometry, Material data (defining surface properties)
	And maybe a shader
	*/

	class Mesh
	{
	public:
		Mesh(const OpenGLVertexBuffer& vertex_buffer, const BufferLayout& layout, const Material& material) :
			m_layout{layout}, m_material{material}
		{
			m_vao.AddVertexBuffer(vertex_buffer);
		}

	private:
		BufferLayout m_layout;
		Material m_material;
		OpenGLVertexArray m_vao;
	};
	
}