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

		static Mesh& GetCoordSystemMesh();
		static Mesh& GetGridMesh();
		static Mesh& GetCubeMesh();
		static Mesh& GetQuadMesh();
		static Mesh& GetQuadMeshTM();
		
		static void Init();

	private:

		static void CreateCoordSystemMesh(float size = 20.0f);
		static void CreateGridMesh(float grid_size = 20.0f);
		static void CreateCubeMesh();
		static void CreateQuadMesh();
		static void CreateQuadMeshTM();
		
		static std::unordered_map<std::string, Mesh> s_mesh_primitive_repo;

	};

}