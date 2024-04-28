#pragma once

#include "elysian/kernal/base.h"
#include "elysian/events/events.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/scene/entity.h"
#include "elysian/camera/perspective_camera_controller.h"



namespace ely {

	class Scene
		{
		public:
		
			Scene();
			~Scene();

			Entity CreateEntity(const std::string& name = ""s);
			Entity CreateGridEntity();
			Entity CreateBoxEntity();
			Entity CreateOrbitingCubeEntity(const glm::vec3& position, const std::string& name);
			Entity CreateQuadEntity(const glm::vec3& position, const std::string& name);
			Entity CreateQuadEntity2(const glm::vec3& position, const std::string& name); //for editor
			Entity CreateDrirectionalLightEntity(const glm::vec3& position, const std::string& name);
			Entity CreatePerspectiveCameraEntity(const glm::vec3& position, const std::string& name);

			Entity FindEntityByName(std::string_view name);
			Entity GetEntityByUUID(UUID uuid);
			void DestroyEntity(Entity entity);

			void BeginScene(const std::string& camera_name, const glm::vec4& clear_color);
			void EndScene();

			void RenderScene();
			void UpdateScene(double time_step);

			void SetRenderable(Entity& entity, bool val);
			void DisplayCoords(Entity& entity, bool val);
			 
			auto& GetRegistry() { return m_registry; }
			void SetControlledCameraEntity(Entity camera_entity);
			Entity GetControlledCameraEntity() { return m_controlled_camera_entity; }
			PerspectiveCameraController& GetCameraController() { return m_camera_controller; }

			void UploadCameraDataToShaders(const std::string& camera_name);
			void UploadLightDataToShader();

			bool OnMouseMoved(ely::EventMouseMoved& e);
			bool OnMouseScrolled(ely::EventMouseScrolled& e);
			bool OnMouseButtonPressed(ely::EventMouseButtonPressed& e);
			bool OnWindowResize(ely::EventWidowResize& e);
			bool OnViewportResize(ely::EventViewportResize& e);

			void SetViewportSize(const glm::vec2& size) { m_viewport_size = size; }

		private:
			entt::registry m_registry;
			std::unordered_map<UUID, entt::entity> m_entity_map; //TODO - what's this used for!? - might be better to use the entity name as the key
			PerspectiveCameraController m_camera_controller; //TODO - make as an component attached to camera entity?
			Entity m_controlled_camera_entity;

			//TEMP - for CreateQuadEntity2
			glm::vec2 m_viewport_size{ 0,0 };

			//TODO needed?
			//friend class Entity;
			//friend class SceneHeirachyPanel;  //this is defined in the sanfbax app - don't really want in the engine?
		};

}


	
