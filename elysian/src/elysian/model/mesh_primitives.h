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
		enum class DataType { VERTICIES, UVS, NORMALS, TEXTURE_MAPPED};

		static OpenGLVertexBuffer* const GetCubeVertexBuffer(DataType data_type = DataType::TEXTURE_MAPPED);
		static OpenGLVertexBuffer* const GetGridVertexBuffer(float grid_size, float unit_size);
		static OpenGLVertexBuffer* const GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size);

		static OpenGLVertexBuffer* const GetSquareXZVertexBuffer();
		static OpenGLVertexBuffer* const GetGrayScaleStripBuffer(bool gamma_corrected);

		//static Ref<Mesh> GetMesh(PrimitiveType primitive_type);

		static Ref<Mesh> GetCubeMesh();

	private:
		
	};

}