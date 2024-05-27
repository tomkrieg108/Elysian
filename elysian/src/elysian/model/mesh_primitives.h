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

		static Mesh& GetCoordSystemMesh1();
		static Mesh& GetGridMesh1();
		static Mesh& GetCubeMesh1();
		static Mesh& GetQuadMesh1();
		static Mesh& GetQuadMeshTM1();
		
		
		static mesh_v2::Mesh& GetCoordSystemMesh2();
		static mesh_v2::Mesh& GetGridMesh2();
		static mesh_v2::Mesh& GetCubeMesh2();
		static mesh_v2::Mesh& GetQuadMesh2();
		static mesh_v2::Mesh& GetQuadMeshTM2();
		
		static void Init();

	private:

		static void CreateCoordSystemMesh1(float size = 20.0f);
		static void CreateGridMesh1(float grid_size = 20.0f);
		static void CreateCubeMesh1();
		static void CreateQuadMesh1();
		static void CreateQuadMeshTM1();
		
		static void CreateCoordSystemMesh2(float size = 20.0f);
		static void CreateGridMesh2(float grid_size = 20.0f);
		static void CreateCubeMesh2();
		static void CreateQuadMesh2();
		static void CreateQuadMeshTM2();
		
		static std::unordered_map<std::string, mesh_v1::Mesh> s_mesh_primitive_repo_v1;
		static std::unordered_map<std::string, mesh_v2::Mesh> s_mesh_primitive_repo_v2;

	};

}