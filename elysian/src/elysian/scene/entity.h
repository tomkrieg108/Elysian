#pragma once
#include "elysian/kernal/base.h"
#include "elysian/scene/component.h"
#include "elysian/model/mesh.h"
#include <glm/glm.hpp>

using namespace std::literals;

namespace ely
{
//#if (SCENE_VERS == 1)
	inline namespace scene_v1 {

		class Entity
		{
		public:

			Entity() = default;
			Entity(const std::string& name) :
				m_name{ name }
			{}

			void AddComponent(Ref<Component> component)
			{
				//UUID component_id = component->GetUUID();
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
			//UUID m_uuid;
			//std::unordered_map <UUID, Ref<Component>> m_components;
			std::unordered_map <std::string, Ref<Component>> m_components;

		};
	}
//#endif
}
 
//#include "elysian/scene/scene.h"
#include <entt/entt.hpp>

namespace ely {

//#if (SCENE_VERS == 2)
	namespace scene_v2 {
	
		class Entity
		{
			//TODO include assert m_registry not null where needed,
			//think about changing the m_registry pointer to Scene pointer

		public:
			Entity() = default;
			Entity(entt::registry* registry) :
				m_entity_handle{entt::null},
				m_registry{ registry }
			{
				m_entity_handle = m_registry->create();
			}

			Entity(entt::entity handle, entt::registry* registry) :
				m_entity_handle{ handle },
				m_registry{ registry }
			{}

			~Entity() = default;

			bool IsValid() { return m_registry->valid(m_entity_handle); }

			template <typename T, typename... Args>
			T& AddComponent(Args&&... args)
			{
				T& component = m_registry->emplace<T>(m_entity_handle, std::forward<Args>(args)...);
				return component;
			}

			template <typename T, typename... Args>
			T& AddOrReplaceComponent(Args&&... args)
			{
				T& component = m_registry->emplace_or_replace<T>(m_entity_handle, std::forward<Args>(args)...);
				return component;
			}

			template <typename T>
			bool HasComponent()
			{
				return m_registry->any_of<T>(m_entity_handle);
			}

			template <typename T>
			T& GetComponent()
			{
				T& component = m_registry->get<T>(m_entity_handle);
				return component;
			}

			template <typename T>
			void RemoveComponent()
			{
				m_registry->remove<T>(m_entity_handle);
			}

			//casts
			operator entt::entity() const{ return m_entity_handle; }
			operator std::int32_t() const { return (std::int32_t)m_entity_handle; }
			operator bool() const { return m_entity_handle != entt::null; }

			UUID GetUUID() { return GetComponent<IDComponent>(); }
			const std::string& GetName() { return GetComponent<TagComponent>(); }

		private:
			entt::entity m_entity_handle = entt::null;
			entt::registry* m_registry = nullptr;
		};

	}

//#endif

}