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
		//TODO  better to return a unique_ptr
		static Ref<OpenGLVertexBuffer> GetCubeVertexBuffer();
		static Ref<OpenGLVertexBuffer> GetGridVertexBuffer(float grid_size, float unit_size);
		static Ref<OpenGLVertexBuffer> GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size);
		static Ref<OpenGLVertexBuffer> GetCoordSystemVertexBuffer(float size);
		static Ref<OpenGLVertexBuffer> GetSquareXZVertexBuffer();
		static Ref<OpenGLVertexBuffer> GetGrayScaleStripBuffer(bool gamma_corrected);
		static Ref<Mesh> GetCubeMesh();
		static Ref<Mesh> GetQuadMesh();
		static Ref<Mesh> GetGridMesh(float grid_size, float unit_size);
		static Ref<Mesh> GetCoordSystemMesh(const glm::mat4& model_mat, float size);

		static Mesh GetGridMesh(float grid_size);
		static Mesh GetCoordSystemMesh(float size = 1.0f);
		static Mesh GetCubeMesh1();
		static Mesh GetQuadMesh1();
		
	private:
		
	};

}