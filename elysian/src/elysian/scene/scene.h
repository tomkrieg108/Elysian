#pragma once

#include "elysian/kernal/base.h"
#include "elysian/events/events.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/camera/perspective_camera_controller.h"
#include "entity.h"

namespace ely
{
#if (SCENE_VERS == 1)

	class Scene
	{
	public:
		Scene() = default;
		~Scene()
		{
			delete m_camera_controller;
		}

		void Init();

		void Begin(Ref<Entity> camera_entity, const glm::vec4& clear_color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
		void Begin(Ref<Entity> camera_entity, const OpenGLFramebuffer& framebuffer,
			const glm::vec4& clear_color = glm::vec4(0.05f, 0.05f, 0.2f, 1.0f));
		void End() {}
		void OnUpdate(double delta_time);
		void OnEvent(Event& event);
		void Render();

		const auto& GetEntities() const { return m_entities; }
		auto& GetEntities() { return m_entities; }
		Ref<Entity> GetEntity(const ::std::string& name);
		auto GetCameraController() { return m_camera_controller; }

	private:
		void LoadCameraData(Ref<Entity> camera_entity);

	private:
		::std::vector<Ref<Entity>> m_entities;
		PerspectiveCameraController* m_camera_controller = nullptr;
	};

#endif
}

#include <entt/entt.hpp>

namespace ely {

#if (SCENE_VERS == 2)

	class Scene
	{
	public:
		Scene();
		~Scene();
			
	private:
		entt::registry m_registry;

	};

#endif
}


	
