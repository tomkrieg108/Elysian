#include "pch.h"
#include <GL/glew.h>
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/kernal/log.h"
#include "mesh_basic.h"
#include <cmath>

namespace ely
{
  static float cube_vertices[] = 
  {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
  };

  static float cube_normals[] = 
  {
     0.0f,  0.0f, -1.0f,  
     0.0f,  0.0f, -1.0f,  
     0.0f,  0.0f, -1.0f,  
     0.0f,  0.0f, -1.0f,  
     0.0f,  0.0f, -1.0f,  
     0.0f,  0.0f, -1.0f,  

     0.0f,  0.0f,  1.0f,  
     0.0f,  0.0f,  1.0f,  
     0.0f,  0.0f,  1.0f,  
     0.0f,  0.0f,  1.0f,  
     0.0f,  0.0f,  1.0f,  
     0.0f,  0.0f,  1.0f,  

    -1.0f,  0.0f,  0.0f,  
    -1.0f,  0.0f,  0.0f,  
    -1.0f,  0.0f,  0.0f,  
    -1.0f,  0.0f,  0.0f,  
    -1.0f,  0.0f,  0.0f,  
    -1.0f,  0.0f,  0.0f,  

     1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,  
     1.0f,  0.0f,  0.0f,  

     0.0f, -1.0f,  0.0f,  
     0.0f, -1.0f,  0.0f,  
     0.0f, -1.0f,  0.0f,  
     0.0f, -1.0f,  0.0f,  
     0.0f, -1.0f,  0.0f,  
     0.0f, -1.0f,  0.0f,  

     0.0f,  1.0f,  0.0f,  
     0.0f,  1.0f,  0.0f,  
     0.0f,  1.0f,  0.0f,  
     0.0f,  1.0f,  0.0f,  
     0.0f,  1.0f,  0.0f,  
     0.0f,  1.0f,  0.0f,  
  };

  static float cube_uvs[] = 
  {
      0.0f,  0.0f,
      1.0f,  0.0f,
      1.0f,  1.0f,
      1.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  0.0f,

      0.0f,  0.0f,
      1.0f,  0.0f,
      1.0f,  1.0f,
      1.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  0.0f,

      1.0f,  0.0f,
      1.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  0.0f,
      1.0f,  0.0f,

      1.0f,  0.0f,
      1.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  1.0f,
      0.0f,  0.0f,
      1.0f,  0.0f,

      0.0f,  1.0f,
      1.0f,  1.0f,
      1.0f,  0.0f,
      1.0f,  0.0f,
      0.0f,  0.0f,
      0.0f,  1.0f,

      0.0f,  1.0f,
      1.0f,  1.0f,
      1.0f,  0.0f,
      1.0f,  0.0f,
      0.0f,  0.0f,
      0.0f,  1.0f
  };

  static float cube_texture_mapped[] =
  {
    // positions          // normals           // texture coords
 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
 -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
 -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
 -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  };

  static float plane_texture_mapped[] =
  {
    0.0f,  0.0f,  0.0f ,  0.0f, 0.0f ,
    1.0f,  0.0f,  1.0f ,  1.0f, 1.0f ,
    0.0f,  0.0f,  1.0f ,  0.0f, 1.0f ,
    0.0f,  0.0f,  0.0f ,  0.0f, 0.0f ,
    1.0f,  0.0f,  0.0f ,  1.0f, 0.0f ,
    1.0f,  0.0f,  1.0f ,  1.0f, 1.0f
  };


  static float square_xz[] =
  {
     0.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  1.0f,
     1.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  1.0f,
     0.0f,  0.0f,  0.0f,
  };

  
  MeshBasic::MeshBasic(const std::vector<float>& vertex_data, const BufferLayout& layout) :
    m_vertex_buffer{ new OpenGLVertexBuffer((void*)vertex_data.data(), vertex_data.size()*sizeof(float), layout)}
	{
	}

  MeshBasic::~MeshBasic()
	{
    delete m_vertex_buffer;
	}

  OpenGLVertexBuffer* const MeshPrimitive::GetCubeVertexBuffer(DataType data_type)
  {
    BufferLayout layout;
    OpenGLVertexBuffer* vertex_buffer = nullptr;
    if (data_type == DataType::VERTICIES)
    {
      layout.push<float>(3);
      vertex_buffer = new OpenGLVertexBuffer((void*)cube_vertices, sizeof(cube_vertices), layout);
    }
      
    else if (data_type == DataType::UVS)
    {
      layout.push<float>(2);
      vertex_buffer = new OpenGLVertexBuffer((void*)cube_uvs, sizeof(cube_uvs), layout);
    }
    else if (data_type == DataType::NORMALS)
    {
      layout.push<float>(3);
      vertex_buffer = new OpenGLVertexBuffer((void*)cube_normals, sizeof(cube_normals), layout);
    }
    else
    {
      layout.push<float>(3); //positions
      layout.push<float>(3); //normals
      layout.push<float>(2); //UV's
      vertex_buffer = new OpenGLVertexBuffer((void*)cube_texture_mapped, sizeof(cube_texture_mapped), layout);
    }
    return vertex_buffer;
  }

  OpenGLVertexBuffer* const MeshPrimitive::GetSquareXZVertexBuffer()
  {
    BufferLayout layout;
    OpenGLVertexBuffer* vertex_buffer = nullptr;

    layout.push<float>(3);
    vertex_buffer = new OpenGLVertexBuffer((void*)square_xz, sizeof(square_xz), layout);
    return vertex_buffer;
  }

  OpenGLVertexBuffer* const MeshPrimitive::GetGrayScaleStripBuffer(bool gamma_corrected)
  {
    std::vector<float> vertices;

    float col = 0.0f;
    const float col_delta = 1.0f / 32.0f;
    
    const auto size = sizeof(square_xz) / sizeof(float);
    const float gamma = gamma_corrected ? 2.2f : 1.0f;
    const float inv_gamma = 1.0f / gamma;

    for (float x = -16.0f; x < 16.0f; x++)
    {
      float col_2 = ::pow(col, inv_gamma);
      for (auto i = 0; i < size; i += 3)
      {
        vertices.push_back(square_xz[i] + x);      //x
        vertices.push_back(square_xz[i + 1]);  //y
        vertices.push_back(square_xz[i + 2]);  //z
        vertices.insert(std::cend(vertices), { col_2,col_2,col_2,1.0f });
      }
      col += col_delta;
    }
    BufferLayout layout;
    layout.push<float>(3); //position
    layout.push<float>(4); //colour
    return new OpenGLVertexBuffer(vertices.data(), vertices.size() * sizeof(float), layout);
  }

  OpenGLVertexBuffer* const MeshPrimitive::GetGridVertexBuffer(float grid_size, float unit_size)
  {
    std::vector<float> vertices;
    const float col = 0.5f; //colour
    const float y = -0.01f;
    float x, z;
    
    z = -grid_size;
    while (z < grid_size + 0.1f)
    {
      x = -grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      x = +grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      z += unit_size;
    }

    x = -grid_size;
    while (x < grid_size + +0.1f)
    {
      z = -grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      z = +grid_size;
      vertices.insert(std::cend(vertices), { x,y,z, col,col,col,1.0f });
      x += unit_size;
    }
    BufferLayout layout;
    layout.push<float>(3); //position
    layout.push<float>(4); //colour
    return new OpenGLVertexBuffer(vertices.data(), vertices.size() * sizeof(float), layout);
  }

  OpenGLVertexBuffer* const MeshPrimitive::GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size)
  {
    std::vector<float> vertices;
    //model_mat is the model matrix of the object that the coords apply to (column major format => mat[col][row])
    const glm::mat4& m = model_mat;
    const float s = size;

    vertices.insert(std::cend(vertices), { 0,0,0, 1,0,0,1 }); //origin x
    vertices.insert(std::cend(vertices), { s * m[0][0], s * m[0][1], s * m[0][2], 1,0,0,1 }); //terminal x

    vertices.insert(std::cend(vertices), { 0,0,0, 0,1,0,1 }); //origin y
    vertices.insert(std::cend(vertices), { s * m[1][0], s * m[1][1], s * m[1][2], 0,1,0,1 }); //terminal y

    vertices.insert(std::cend(vertices), { 0,0,0, 0,0,1,1 }); //origin z
    vertices.insert(std::cend(vertices), { s * m[2][0], s * m[2][1], s * m[2][2], 0,0,1,1 }); //terminal z

    BufferLayout layout;
    layout.push<float>(3); //position
    layout.push<float>(4); //colour
    return new OpenGLVertexBuffer(vertices.data(), vertices.size() * sizeof(float), layout);
  }

}