#include "pch.h"
#include "elysian/renderer/opengl_renderer.h"
#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{

	CubeEntity::CubeEntity(Ref<Mesh> mesh) :
		Entity(mesh)
	{}

	CubeEntity::CubeEntity(const glm::mat4& transform, Ref<Mesh> mesh) :
		Entity(transform, mesh)
	{}

	void CubeEntity::OnUpdate(double delta_time)
	{
		//rotate the box on local axis
		glm::vec3 rot_axis = glm::vec3(1, 1, 1);
		float angle = (float)delta_time; // 1/60 radians 
		m_transform_mat = glm::rotate(m_transform_mat, angle, rot_axis);

		//orbit box about world y axis
		glm::mat4 orbit_axis = glm::mat4(1.0f);
		orbit_axis = glm::rotate(orbit_axis, angle, glm::vec3(0, 1, 0));
		m_transform_mat = orbit_axis * m_transform_mat;
	}

	void CubeEntity::Draw()
	{
		ely::OpenGLRenderer::DrawMesh(GetMesh(), ely::ShaderRepo::Get("light_map_diff_spec"));
	}

}