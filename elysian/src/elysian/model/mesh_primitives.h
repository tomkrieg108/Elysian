#pragma once
#include "elysian/kernal/base.h"
#include <glm/glm.hpp>
#include "elysian/renderer/opengl_buffer.h"
#include "mesh.h"

namespace ely
{
	enum class PrimitiveType
	{
		Quad, Cube, Line, Grid
	};
	
	class MeshPrimitive
	{
	public:
		
		static OpenGLVertexBuffer GetCubeVertexBuffer();
		static OpenGLVertexBuffer GetGridVertexBuffer(float grid_size, float unit_size);
		static OpenGLVertexBuffer GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size);
		static OpenGLVertexBuffer GetCoordSystemVertexBuffer(float size);
		static OpenGLVertexBuffer GetSquareXZVertexBuffer();
		static OpenGLVertexBuffer GetGrayScaleStripBuffer(bool gamma_corrected);

#if 0 
		static Ref<Mesh> GetCubeMesh();
		static Ref<Mesh> GetQuadMesh();
		static Ref<Mesh> GetGridMesh(float grid_size, float unit_size);
		static Ref<Mesh> GetCoordSystemMesh(const glm::mat4& model_mat, float size);
#endif

		static Mesh GetCubeMesh1();
		static Mesh GetQuadMesh1();
		static Mesh GetGridMesh1(float grid_size);
		static Mesh GetCoordSystemMesh1(float size = 1.0f);

		static mesh_v2::Mesh GetCubeMesh2();
		static mesh_v2::Mesh GetQuadMesh2();
		static mesh_v2::Mesh GetGridMesh2(float grid_size = 20.0f);
		static mesh_v2::Mesh GetCoordSystemMesh2(float size = 1.0f);
			
	};

}