#pragma once
#include "elysian/kernal/base.h"
#include "elysian/kernal/uuid.h"
#include "elysian/scene/component.h"
#include "Elysian/events/events.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/model/material.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/model/mesh.h"
#include "elysian/light/directional_light.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include <glm/glm.hpp>


using namespace std::literals;

namespace ely
{
#if (SCENE_VERS == 1)

		class Entity
		{
		public:

			Entity() = default;
			Entity(const std::string& name) :
				m_name{ name }
			{}

			void AddComponent(Ref<Component> component)
			{
				UUID component_id = component->GetUUID();
				//m_components[component_id] = component;
				//CORE_TRACE("Component ref count before insertion to m_compoents {}", component.use_count()); 
				//pretty dubious code right here!
				if (std::dynamic_pointer_cast<TagComponent>(component))
					m_components["tag"s] = component;
				else if (std::dynamic_pointer_cast<TransformComponent>(component))
					m_components["transform"s] = component;
				else if (std::dynamic_pointer_cast<MeshComponent>(component))
					m_components["mesh"s] = component; //TODO - could have multiple meshes?
				else if (std::dynamic_pointer_cast<ShaderComponent>(component))
					m_components["shader"s] = component;
				else if (std::dynamic_pointer_cast<BehaviourComponent>(component))
					m_components["behaviour"s] = component;
				else if (std::dynamic_pointer_cast<EventHandlerComponent>(component))
					m_components["event_handler"s] = component;
				else if (std::dynamic_pointer_cast<LightComponent>(component))
					m_components["light"s] = component;
				else if (std::dynamic_pointer_cast<PerspectiveCameraComponent>(component))
					m_components["camera"s] = component;

				//CORE_TRACE("Component ref count adter insertion to m_compoents {}", component.use_count());
			}

			void RemoveMeshComponent()
			{
				//somewhat dubious again
				if (m_components.find("mesh") != m_components.end())
					m_components.erase("mesh");
			}

			void AddMeshComponent()
			{
				//somewhat dubious again
				if (m_components.find("mesh") == m_components.end())
				{
					auto camera_mesh = MeshPrimitive::GetCubeMesh();
					camera_mesh->SetMaterial(*MaterialRepo::Get("empty"));
					auto camera_mesh_comp = CreateRef< MeshComponent>(*this, camera_mesh);
					m_components["mesh"s] = camera_mesh_comp;
				}
			}

			void AddMeshComponent(Ref<MeshComponent> mesh_component)
			{
				//somewhat dubious again
				if (m_components.find("mesh") == m_components.end())
				{
					m_components["mesh"s] = mesh_component;
				}
			}

			void Render();
			void OnUpdate(double time_step);
			void OnEvent(Event& event);

			const auto& GetComponents() const { return m_components; }
			auto& GetComponents() { return m_components; }
			const auto& GetName() const { return m_name; }

			Ref<Component> GetComponent(const std::string& name)
			{
				if (m_components.find(name) != m_components.end())
					return m_components[name];

				return nullptr;
			}

			auto begin() const { return std::cbegin(m_components); }
			auto end() const { return std::cend(m_components); }

		private:
			std::string m_name = "Entity";
			UUID m_uuid;
			//std::unordered_map <UUID, Ref<Component>> m_components;
			std::unordered_map <std::string, Ref<Component>> m_components;

		};
#endif
}

namespace ely {

#if (SCENE_VERS == 2)

#endif

}