#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include "elysian/renderer/opengl_buffer_layout.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/kernal/log.h"
#include "mesh_primitives.h"
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

 //note that sizeof() operator returns size_t which is a 64 bit unsigned int
  OpenGLVertexBuffer* const MeshPrimitive::GetCubeVertexBuffer(DataType data_type)
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_tex_coords", ShaderDataType::Float2},
    };

    OpenGLVertexBuffer* vertex_buffer = new OpenGLVertexBuffer((void*)cube_texture_mapped, (int32_t)sizeof(cube_texture_mapped), layout);
    return vertex_buffer;
  }

  OpenGLVertexBuffer* const MeshPrimitive::GetSquareXZVertexBuffer()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3}
    };
    OpenGLVertexBuffer* vertex_buffer = new OpenGLVertexBuffer((void*)square_xz, (int32_t)sizeof(square_xz), layout);
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
    return new OpenGLVertexBuffer(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);

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

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };

    return new OpenGLVertexBuffer(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);
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

    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_color", ShaderDataType::Float4}
    };
    return new OpenGLVertexBuffer(vertices.data(), (int32_t)(vertices.size() * sizeof(float)), layout);
  }

  /*Ref<Mesh> MeshPrimitive::GetMesh(PrimitiveType primitive_type)
  {
    auto mesh = CreateRef<Mesh>();
    return mesh;
  }*/

 Ref<Mesh> MeshPrimitive::GetCubeMesh()
  {
    MaterialSpecification material_spec =
    {
      {"u_albedo_map", ShaderDataType::Sampler2D},
      {"u_specular_map", ShaderDataType::Sampler2D},
      {"u_shininess", ShaderDataType::Float}
    };

    MaterialData material_data =
    {
      {"u_albedo_map", (uint32_t)0}, //without the case its interpreted as int
      {"u_specular_map", (uint32_t)1},
      {"u_shininess", 32.0f}
    };

    MaterialData material_data2;
    material_data.SetValue("u_albedo_map", (uint32_t)0)
      .SetValue("u_specular_map", (uint32_t)1)
      .SetValue("u_specular_map", 32.0f);
    
    Material material(ShaderRepo::Get("cube_shader"), material_spec, material_data);

    BufferLayout buffer_layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_tex_coords", ShaderDataType::Float2},
    };

    OpenGLVertexBuffer vbo = OpenGLVertexBuffer((void*)cube_texture_mapped, (int32_t)sizeof(cube_texture_mapped), buffer_layout);

    auto mesh = CreateRef<Mesh>(vbo, buffer_layout, material);

    return mesh;
  }
}