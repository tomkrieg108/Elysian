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

  std::unordered_map<std::string, mesh_v1::Mesh> MeshPrimitive::s_mesh_primitive_repo_v1{};
  std::unordered_map<std::string, mesh_v2::Mesh> MeshPrimitive::s_mesh_primitive_repo_v2{};
 
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

  static float square_xz_tm[] =
  {
    // positions          // normals          // uv coords
     0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 
     1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
     0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
     0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
  };

  /*
    TODO and NOTE!!! since these buffers are getting returned by value in these functions, as soon as the function
    returns, the local buffer object returned goes out of scope and it's destructor is called.  If the constructor 
    calls glDeleteBuffers() the returned buffer is immediately broken and will not render
  */
  

 //note that sizeof() operator returns size_t which is a 64 bit unsigned int
  OpenGLVertexBuffer MeshPrimitive::GetCubeVertexBuffer()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    auto vertex_buffer = OpenGLVertexBuffer{ (void*)cube_vertices, (int32_t)sizeof(cube_vertices), layout };
    return vertex_buffer;
  }

  OpenGLVertexBuffer MeshPrimitive::GetSquareXZVertexBuffer()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3}
    };
    auto vertex_buffer = OpenGLVertexBuffer{ (void*)square_xz, (int32_t)sizeof(square_xz), layout };
    return vertex_buffer;
  }

  OpenGLVertexBuffer MeshPrimitive::GetGrayScaleStripBuffer(bool gamma_corrected)
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

    auto vertex_buffer = OpenGLVertexBuffer{ vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(float)), layout };
    return vertex_buffer;
  }

  OpenGLVertexBuffer MeshPrimitive::GetGridVertexBuffer(float grid_size, float unit_size)
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
    auto vertex_buffer = OpenGLVertexBuffer{ vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(float)), layout };
    return vertex_buffer;
  }

  OpenGLVertexBuffer MeshPrimitive::GetCoordSystemVertexBuffer(const glm::mat4& model_mat, float size)
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
    auto vertex_buffer = OpenGLVertexBuffer{ vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(float)), layout };
    return vertex_buffer;
  }

  OpenGLVertexBuffer MeshPrimitive::GetCoordSystemVertexBuffer(float size)
  {
    //Note - no need really to pass a mat4 - it's going to get trasformed according to the transform component anyway
    std::vector<float> vertices;
    //model_mat is the model matrix of the object that the coords apply to (column major format => mat[col][row])
    const glm::mat4& m{ glm::mat4(1.0f) };
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
    auto vertex_buffer = OpenGLVertexBuffer{ vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(float)), layout };
    return vertex_buffer;
  }

  //=================================================================================

  Mesh& MeshPrimitive::GetCoordSystemMesh1()
  {
    return s_mesh_primitive_repo_v1["coords"];
  }

  Mesh& MeshPrimitive::GetGridMesh1()
  {
    return s_mesh_primitive_repo_v1["grid"];
  }

  Mesh& MeshPrimitive::GetCubeMesh1()
  {
    return s_mesh_primitive_repo_v1["cube"];
  }

  Mesh& MeshPrimitive::GetQuadMesh1()
  {
    return s_mesh_primitive_repo_v1["quad"];
  }

  Mesh& MeshPrimitive::GetQuadMeshTM1()
  {
    return s_mesh_primitive_repo_v1["quad_tm"];
  }

  //=================================================================================

  mesh_v2::Mesh& MeshPrimitive::GetCoordSystemMesh2()
  {
    return s_mesh_primitive_repo_v2["coords"];
  }

  mesh_v2::Mesh& MeshPrimitive::GetGridMesh2()
  {
    return s_mesh_primitive_repo_v2["grid"];
  }

  mesh_v2::Mesh& MeshPrimitive::GetCubeMesh2()
  {
    return s_mesh_primitive_repo_v2["cube"];
  }

  mesh_v2::Mesh& MeshPrimitive::GetQuadMesh2()
  {
    return s_mesh_primitive_repo_v2["quad"];
  }

  mesh_v2::Mesh& MeshPrimitive::GetQuadMeshTM2()
  {
    return s_mesh_primitive_repo_v2["quad_tm"];
  }

  //=================================================================================

  void MeshPrimitive::CreateCoordSystemMesh1(float size)
  {
    auto vbo = MeshPrimitive::GetCoordSystemVertexBuffer(size);
    auto material = MaterialRepo::Get("empty");
    Mesh mesh{ vbo, *material, DrawMode::Lines };
    s_mesh_primitive_repo_v1["coords"] = mesh;
  }

  void MeshPrimitive::CreateGridMesh1(float grid_size)
  {
    auto vbo = MeshPrimitive::GetGridVertexBuffer(grid_size, 1.0f); //TODO vbo destructor immediately called in GetGridVertexBuffer => glBufferDestroed() => vbo invalid
    auto material = MaterialRepo::Get("empty");
    Mesh mesh{ vbo, *material, DrawMode::Lines };
    s_mesh_primitive_repo_v1["grid"] = mesh;
  }

  void MeshPrimitive::CreateCubeMesh1()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    OpenGLVertexBuffer vbo{ (void*)cube_vertices, (int32_t)sizeof(cube_vertices), layout };
    auto material = MaterialRepo::Get("container2_specular");
    auto mesh = Mesh{ vbo, *material, DrawMode::Triangles };
    s_mesh_primitive_repo_v1["cube"] = mesh;
  }

  void MeshPrimitive::CreateQuadMesh1()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
    };
    OpenGLVertexBuffer vbo{ (void*)square_xz, (int32_t)sizeof(square_xz), layout };
    auto material = MaterialRepo::Get("colored_basic_yellow");
    auto mesh = Mesh{ vbo, *material, DrawMode::Triangles };
    s_mesh_primitive_repo_v1["quad"] = mesh;
  }

  void MeshPrimitive::CreateQuadMeshTM1()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    OpenGLVertexBuffer vbo{ (void*)square_xz_tm, (int32_t)sizeof(square_xz_tm), layout };
    auto material = MaterialRepo::Get("container2_specular");
    auto mesh = Mesh{ vbo, *material, DrawMode::Triangles };
    s_mesh_primitive_repo_v1["quad_tm"] = mesh;
  }
  
  //=================================================================================

  void MeshPrimitive::CreateCoordSystemMesh2(float size)
  {
    auto vbo = MeshPrimitive::GetCoordSystemVertexBuffer(size);
    auto mesh = mesh_v2::Mesh{ vbo };
    mesh.SetDrawMode(DrawMode::Lines);
    s_mesh_primitive_repo_v2["coords"] = mesh;
  }

  void MeshPrimitive::CreateGridMesh2(float grid_size)
  {
    auto vbo = MeshPrimitive::GetGridVertexBuffer(grid_size, 1.0f);
    auto mesh = mesh_v2::Mesh{ vbo };
    mesh.SetDrawMode(DrawMode::Lines);
    s_mesh_primitive_repo_v2["grid"] = mesh;
  }

  void MeshPrimitive::CreateCubeMesh2()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    OpenGLVertexBuffer vbo{ (void*)cube_vertices, (int32_t)sizeof(cube_vertices), layout };
    auto mesh = mesh_v2::Mesh{ vbo };
    s_mesh_primitive_repo_v2["cube"] = mesh;
  }

  void MeshPrimitive::CreateQuadMesh2()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
    };
    OpenGLVertexBuffer vbo{ (void*)square_xz, (int32_t)sizeof(square_xz), layout };
    auto mesh = mesh_v2::Mesh{ vbo };
    s_mesh_primitive_repo_v2["quad"] = mesh;
  }

  void MeshPrimitive::CreateQuadMeshTM2()
  {
    BufferLayout layout =
    {
      {"a_position", ShaderDataType::Float3},
      {"a_normal", ShaderDataType::Float3},
      {"a_uv_coords", ShaderDataType::Float2},
    };
    OpenGLVertexBuffer vbo{ (void*)square_xz_tm, (int32_t)sizeof(square_xz_tm), layout };
    auto mesh = mesh_v2::Mesh{ vbo };
    s_mesh_primitive_repo_v2["quad_tm"] = mesh;
  }

  //=================================================================================

  void MeshPrimitive::Init()
  {
    CreateCoordSystemMesh1();
    CreateGridMesh1();
    CreateCubeMesh1();
    CreateQuadMesh1();

    CreateCoordSystemMesh2();
    CreateGridMesh2();
    CreateCubeMesh2();
    CreateQuadMesh2();
  }

}