#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_draw_mode.h"
#include "elysian/kernal/log.h"
#include "mesh_primitives.h"
#include <cmath>

namespace ely
{
 
  static float cube_vertices[] =
  {
    // positions          // normals          // uv coords
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

  
  static float quad_vertices[] = //X-Z plane
  {
    // positions          // normals          // uv coords
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

 //note that sizeof() operator returns size_t which is a 64 bit unsigned int
  Ref<OpenGLVertexBuffer> MeshPrimitive::GetCubeVertexBuffer()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    auto vertex_buffer = CreateRef<OpenGLVertexBuffer>((void*)cube_vertices, (int32_t)sizeof(cube_vertices), layout);
    return vertex_buffer;
  }

  Ref<OpenGLVertexBuffer> MeshPrimitive::GetSquareXZVertexBuffer()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3}
    };
    auto vertex_buffer = CreateRef<OpenGLVertexBuffer>((void*)square_xz, (int32_t)sizeof(square_xz), layout);
    return vertex_buffer;
  }

  Ref<OpenGLVertexBuffer> MeshPrimitive::GetGrayScaleStripBuffer(bool gamma_corrected)
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
        vertices.push_back(square_xz[i] + x);  //x
        vertices.push_back(square_xz[i + 1]);  //y
        vertices.push_back(square_xz[i + 2]);  //z
        vertices.insert(std::cend(vertices), { col_2,col_2,col_2,1.0f });
      }
      col += col_delta;
    }

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };

    auto vertex_buffer = CreateRef<OpenGLVertexBuffer>(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);
    return vertex_buffer;
  }

  Ref<OpenGLVertexBuffer> MeshPrimitive::GetGridVertexBuffer(float grid_size, float unit_size)
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

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };
    auto vertex_buffer = CreateRef<OpenGLVertexBuffer>(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);
    return vertex_buffer;
  }

  Ref<OpenGLVertexBuffer> MeshPrimitive::GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size)
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

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };
    auto vertex_buffer = CreateScope<OpenGLVertexBuffer>(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);
    return vertex_buffer;
  }

  Ref<Mesh> MeshPrimitive::GetCubeMesh()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    OpenGLVertexBuffer vbo{ (void*)cube_vertices, (int32_t)sizeof(cube_vertices), layout };
    auto material = MaterialRepo::Get("container2_specular");
    auto mesh = CreateRef<Mesh>(vbo, *material, DrawMode::Triangles);
    return mesh;
  }

  Ref<Mesh> MeshPrimitive::GetQuadMesh()
  {
    auto vbo = MeshPrimitive::GetSquareXZVertexBuffer();
    auto material = MaterialRepo::Get("colored_basic_yellow");
    auto mesh = CreateRef<Mesh>(*vbo, *material, DrawMode::Triangles);
    return mesh;
  }

  Ref<Mesh> MeshPrimitive::GetGridMesh(float grid_size, float unit_size)
  {
    auto vbo = MeshPrimitive::GetGridVertexBuffer(grid_size, unit_size);
    auto material = MaterialRepo::Get("empty");
    auto mesh = CreateRef<Mesh>(*vbo, *material, DrawMode::Lines);
    return mesh;
  }

  Ref<Mesh> MeshPrimitive::GetCoordSystemMesh(const glm::mat4& model_mat, float size)
  {
    auto vbo = MeshPrimitive::GetCoordSystemVertexBuffer(model_mat, size);
    auto material = MaterialRepo::Get("empty");
    auto mesh = CreateRef<Mesh>(*vbo, *material, DrawMode::Lines);
    return mesh;
  }
}