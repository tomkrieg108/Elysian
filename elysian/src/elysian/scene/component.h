#pragma once
#include "elysian/kernal/base.h"
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


	class Entity;

		class Component
		{
			//friend class Entity;
			

		public:
			Component(Entity& parent_entity) :
				m_parent_entity{ parent_entity }
			{
			}

			virtual ~Component() = default;

			virtual void UploadToShader(const Ref<Shader>& shader) const {}

			UUID GetUUID() const { return m_uuid; }

		public:
			Entity& m_parent_entity; //TODO - this could end up being a uuid too?
			UUID m_uuid;
		};

		//------------------------------------------------------

		class TagComponent : public Component
		{
		public:
			TagComponent(Entity& parent_entity, const std::string& tag) :
				Component{ parent_entity }, m_tag{ tag }
			{}
		public:
			std::string m_tag{ ""s };
		};

		//------------------------------------------------------

		class TransformComponent : public Component
		{
		public:
			TransformComponent(Entity& parent_entity, const glm::mat4& transform = glm::mat4{ 1.0f }) :
				Component{ parent_entity }, m_transform{ transform }
			{}

			virtual void UploadToShader(const Ref<Shader>& shader) const override {

				shader->Bind();
				shader->SetUniformMat4f("u_model", m_transform);
			}

			void SetTransform(const glm::mat4& transform) { m_transform = transform; }
			glm::mat4& GetTransform() { return m_transform; }

		public:
			glm::mat4 m_transform = glm::mat4(1.0f);
		};


		//------------------------------------------------------
		//TODO material is currently in the mesh - might want it to be a seperate component - in which case MeshComponent just contains a vao!

		class MeshComponent : public Component
		{
		public:

			MeshComponent(Entity& parent_entity, const Ref<Mesh>& mesh) :
				Component{ parent_entity }, m_mesh{ mesh }
			{}

			void SetMesh(const Ref<Mesh>& mesh) { m_mesh = mesh; }
			auto GetMesh() { return m_mesh; } //might want to change draw mode or material on the mesh, so non-const

			virtual void UploadToShader(const Ref<Shader>& shader) const override
			{
				m_mesh->GetMaterial().UploadDataToShader(shader);
			}

		public:
			Ref<Mesh> m_mesh;
		};

		//TODO - maybe this should be in the mesh after all!
		class ShaderComponent : public Component
		{
		public:
			ShaderComponent(Entity& parent_entity, const Ref<Shader>& shader) :
				Component{ parent_entity }, m_shader{ shader }
			{}
			const auto GetShader() const { return m_shader; }
		public:
			Ref<Shader> m_shader;
		};

		//----------------------------------------------------------------------------

		class LightComponent : public Component
		{
		public:
			LightComponent(Entity& parent_entity, const DirectionalLight& light) :
				Component{ parent_entity }, m_light{ light }
			{}

			void SetLight(const DirectionalLight& light) { m_light = light; }
			auto GetLight() { return m_light; } //might want to change values of light, so non-const

			virtual void UploadToShader(const Ref<Shader>& shader) const override
			{
				m_light.UploadDataToShader(shader);
			}

		public:
			DirectionalLight m_light;
		};

		//----------------------------------------------------------------------------
		class PerspectiveCameraComponent : public Component
		{
		public:
			PerspectiveCameraComponent(Entity& parent_entity, const PerspectiveCamera& camera) :
				Component{ parent_entity }, m_camera{ camera }
			{}
			void SetCamera(const PerspectiveCamera& camera) { m_camera = camera; }
			auto& GetCamera() { return m_camera; } //might want to change values of light, so non-const

		public:
			PerspectiveCamera m_camera;
		};

		//----------------------------------------------------------------------------

		using UpdateFunc = std::function<void(double)>;
		class BehaviourComponent : public Component
		{
		public:
			BehaviourComponent(Entity& parent_entity, UpdateFunc func) :
				Component{ parent_entity }, m_behaviour_func{ func }
			{}
			void OnUpdate(double time_step) { m_behaviour_func(time_step); }

		public:
			UpdateFunc m_behaviour_func;
		};

		//-----------------------------------------------------------------------------
		using OnEventFunc = std::function<void(Event&)>;
		class EventHandlerComponent : public Component
		{
		public:
			EventHandlerComponent(Entity& parent_entity, OnEventFunc func) :
				Component{ parent_entity }, m_event_handler{ func }
			{}

			void OnEvent(Event& event) { m_event_handler(event); }
		public:
			OnEventFunc m_event_handler;
		};

#endif
}

namespace ely
{
#if (SCENE_VERS == 2)

#endif

}