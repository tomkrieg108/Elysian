#pragma once

#include "elysian/kernal/base.h"
#include "elysian/events/events.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "entity.h"
#include "elysian/camera/perspective_camera_controller.h"


namespace ely
{
//#if (SCENE_VERS == 1)

	 inline namespace scene_v1 { 

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

	}

//#endif
}

#include <entt/entt.hpp>

namespace ely {

//#if (SCENE_VERS == 2)
	namespace scene_v2 {

		class Scene
		{
		public:
			Scene();
			~Scene();

			Entity CreateEntity(const std::string& name = ""s);
			Entity CreateGridEntity();
			Entity CreateCoordSysEntity();
			Entity CreateOrbitingCubeEntity(const glm::vec3& position, const std::string& name);
			Entity CreateQuadEntity(const glm::vec3& position, const std::string& name);
			Entity CreateDrirectionalLightEntity(const glm::vec3& position, const std::string& name);
			Entity CreatePerspectiveCameraEntity(const glm::vec3& position, const std::string& name);

			Entity FindEntityByName(std::string_view name);
			Entity GetEntityByUUID(UUID uuid);
			void DestroyEntity(Entity entity);

			void BeginScene(const std::string& camera_name, const glm::vec4& clear_color);
			void BeginScene(const std::string& camera_name, const OpenGLFramebuffer& framebuffer,
				const glm::vec4& clear_color = glm::vec4(0.05f, 0.05f, 0.2f, 1.0f));
			void EndScene();
			void RenderScene();
			void UpdateScene(double time_step);
			void OnEvent(Event& event);

			void SetRenderable(Entity& entity, bool val);
			void DisplayCoords(Entity& entity, bool val);
			 
			auto& GetRegistry() { return m_registry; }
			void SetControlledCameraEntity(Entity camera_entity);
			Entity GetControlledCameraEntity() const { return m_controlled_camera_entity; }
			PerspectiveCameraController& GetCameraController() { return m_camera_controller; }

		private:
			void UploadCameraDataToShaders(const std::string& camera_name);
			void UploadLightDataToShader();

		private:
			entt::registry m_registry;
			std::unordered_map<UUID, entt::entity> m_entity_map;
			PerspectiveCameraController m_camera_controller; //TODO - make as an component attached to camera entity?
			Entity m_controlled_camera_entity;
		};
	}
//#endif
}


	
