#pragma once
#include <glm/glm.hpp>
#include "elysian/renderer/opengl_buffer.h"

namespace ely
{
	//class OpenGLVertexBuffer;
	//class BufferLayout;

	class MeshBasic
	{
	public:
		MeshBasic(const std::vector<float>& vertex_data, const BufferLayout& layout);
		~MeshBasic();

	private:
		OpenGLVertexBuffer* m_vertex_buffer = nullptr;
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
	};

} 