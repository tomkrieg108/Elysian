#pragma once
#include "elysian/kernal/base.h"
#include "elysian/model/material.h"
#include "elysian/model/mesh.h"
#include <glm/glm.hpp>


/*
	Temporary!
	Here's how not to setup entity!!
*/

namespace ely
{
	class Entity
	{
	public:
		
		Entity( Ref<Mesh> mesh) : m_mesh{ mesh }
		{}

		Entity(const glm::mat4& transform, Ref<Mesh> mesh) :
			m_transform_mat{transform}, m_mesh{mesh}
		{}

		virtual ~Entity() = default;

		virtual void OnUpdate(double delta_time) {}
		virtual void Draw() = 0;

		void SetDisplayCoords(bool val) { m_display_coords = val; }
		const glm::mat4& GetTransform() const { return m_transform_mat; }
		auto GetMesh() const { return m_mesh; }
		auto& GetMaterial() { return m_mesh->GetMaterial(); }

	protected:
		glm::mat4 m_transform_mat = glm::mat4(1.0f);
		Ref<Mesh> m_mesh;
		bool m_display_coords = true;
	};

	//--------------------------------------------------------------------------

	class CubeEntity : public Entity
	{
	public: 
	
		CubeEntity(Ref<Mesh> mesh);
		CubeEntity(const glm::mat4& transform, Ref<Mesh> mesh);
		~CubeEntity() = default;

		virtual void OnUpdate(double delta_time) override;
		virtual void Draw() override;

	private:

	};

}