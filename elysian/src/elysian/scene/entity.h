#pragma once
#include "elysian/kernal/base.h"
#include "elysian/scene/component.h"
#include "elysian/model/mesh.h"
#include <glm/glm.hpp>
#include <entt/entt.hpp>

using namespace std::literals;

namespace ely {

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