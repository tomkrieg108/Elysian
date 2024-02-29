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
		
		//TODO - better if these return Ref<OpenGLVertexBuffer> 
		static OpenGLVertexBuffer* const GetCubeVertexBuffer();
		static OpenGLVertexBuffer* const GetGridVertexBuffer(float grid_size, float unit_size);
		static OpenGLVertexBuffer* const GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size);

		static OpenGLVertexBuffer* const GetSquareXZVertexBuffer();
		static OpenGLVertexBuffer* const GetGrayScaleStripBuffer(bool gamma_corrected);

		static Ref<Mesh> GetCubeMesh();
		static Ref<Mesh> GetQuadMesh();
		static Ref<Mesh> GetGridMesh(float grid_size, float unit_size);
		static Ref<Mesh> GetCoordSystemMesh(const glm::mat4& model_mat, float size);
		
	private:
		
	};

}