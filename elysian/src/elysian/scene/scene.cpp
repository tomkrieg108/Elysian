#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/model/mesh_primitives.h"
#include "scene.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{
	void Scene::Init()
	{
		glm::mat4 transform( 1.0f );
		transform = glm::translate(transform, glm::vec3(-2.0f, 2.0f, 5.0f));
		auto cube_mesh = ely::MeshPrimitive::GetCubeMesh();
		auto cube_entity = CreateRef<CubeEntity>(transform, cube_mesh);
		m_entities.push_back(cube_entity);

	}

	void Scene::OnUpdate(double delta_time)
	{
		for (const auto& entity : m_entities)
			entity->OnUpdate(delta_time);
	}

	void Scene::Draw()
	{
		for (const auto& entity : m_entities)
			entity->Draw();
	}


}