#pragma once
#include "elysian/kernal/base.h"
#include "elysian/kernal/uuid.h"
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
//#if (SCENE_VERS == 1)

	inline namespace scene_v1 {

		class Entity;

		class Component
		{
			friend class Entity;


		public: 
			Component(Entity& parent_entity) :
				m_parent_entity{ parent_entity }
			{
			}

			virtual ~Component() = default;

			virtual void UploadToShader(const Ref<Shader>& shader) const {}

			UUID GetUUID() const { return m_uuid; }

		protected:
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
		private:
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

		private:
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

		private:
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

		private:
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

		private:
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

		private:
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
		private:
			OnEventFunc m_event_handler;
		};
	}
//#endif
}

namespace ely
{
//#if (SCENE_VERS == 2)
	namespace scene_v2 {

		class IDComponent
		{
		public:
			//NOTE:  If no constructors are specified at all, compiler will generate default ctr, copy and move constructors
			// when these 2 lines are included, compiler does not generate the move ctr;
			IDComponent() = default;
			IDComponent(const IDComponent& component) = default;
			//explicit operator UUID () const { return m_uuid; }
			operator UUID () const { return m_uuid; }
		public:
			UUID m_uuid;
		};

		//---------------------------------------------------------------

		class TagComponent
		{
		public:
			TagComponent() = default;
			TagComponent(const TagComponent& component) = default;
			TagComponent(const std::string& tag) : m_tag{tag} {}
			//explicit operator const std::string& () const { return m_tag; }
			operator const std::string& () const { return m_tag; }
		public:
			std::string m_tag{ "Entity"s };
		};

		//---------------------------------------------------------------

		class TransformComponent
		{
		public:
			TransformComponent() = default;
			TransformComponent(const TransformComponent& component) = default;
			TransformComponent(const glm::mat4& transform) : m_transform{transform} {}
			void SetTransform(const glm::mat4& transform) { m_transform = transform; }
			//explicit operator glm::mat4& () { return m_transform; }
			//explicit operator const glm::mat4& () const { return m_transform; }
			operator glm::mat4& () { return m_transform; }
			operator const glm::mat4& () const { return m_transform; }

		private:
			glm::mat4 m_transform = glm::mat4(1.0f);
		};

		//---------------------------------------------------------------

		class MeshComponent
		{
			//TODO should perhaps make sure move constructors are availabe too
		public:
			MeshComponent() = default;
			MeshComponent(const MeshComponent& component) = default;
			MeshComponent(const Mesh& mesh) : m_mesh{ mesh } {}

			void SetEnableRender(bool val) { m_enable_render = val; }
			bool GetEnableRender() const { return m_enable_render; }

			void SetShowCoords(bool val) { m_show_coords = val; }
			bool GetShowCoords() const { return m_show_coords; }

			//explicit operator Mesh& () { return m_mesh; }
			//explicit operator const Mesh& () const { return m_mesh; }

			operator Mesh& () { return m_mesh; }
			operator const Mesh& () const { return m_mesh; }

		public:
			Mesh m_mesh;
			bool m_enable_render = true;
			bool m_show_coords = true;
		};

		//---------------------------------------------------------------

		class PerspectiveCameraComponent
		{
		public:
			PerspectiveCameraComponent() = default;
			PerspectiveCameraComponent(const PerspectiveCameraComponent& component) = default;
			PerspectiveCameraComponent(const PerspectiveCamera& camera) : m_camera{ camera } {}

			//explicit operator PerspectiveCamera& () { return m_camera; }
			//explicit operator const PerspectiveCamera& () const { return m_camera; }

			operator PerspectiveCamera& () { return m_camera; }
			operator const PerspectiveCamera& () const { return m_camera; }

		public:
			PerspectiveCamera m_camera;
		};

		//---------------------------------------------------------------

		class DirectionalLightComponent
		{
		public:
			DirectionalLightComponent() = default;
			DirectionalLightComponent(DirectionalLight& light) : m_light {light} {}

			operator DirectionalLight& () { return m_light; }
			operator const DirectionalLight& () const { return m_light; }
		public:
			DirectionalLight m_light;
		};

		//---------------------------------------------------------------

		class ShaderHandleComponent
		{
		public:
			ShaderHandleComponent() = delete;
			ShaderHandleComponent(const ShaderHandleComponent& component) = default;
			ShaderHandleComponent(Shader& shader) : m_shader_handle{shader} {}

			const Shader& GetShader() const { return m_shader_handle.GetShader(); }

			//explicit operator ShaderHandle () const { return m_shader_handle; }
			 operator ShaderHandle () const { return m_shader_handle;}
			//operator uint32_t () const { return (uint32_t)m_shader_handle; }

		public:
			ShaderHandle m_shader_handle;
		};

		//---------------------------------------------------------------

		using UpdateFunc = std::function<void(double)>;
		class UpdatableComponent
		{
		public:
			UpdatableComponent() = default; 
			UpdatableComponent(UpdateFunc func) :
				m_update_func{ func }
			{}
			
			void OnUpdate(double time_step) const { m_update_func(time_step); }
		public:
			UpdateFunc m_update_func; //NOTE can used operator bool to check if contains no callable target
		};


		//-----------------------------------------------------------------------------
		using EventHandler = std::function<void(Event&)>;
		class EventHandlerComponent
		{
		public:
			EventHandlerComponent() = default;
			EventHandlerComponent(OnEventFunc func) : m_event_handler{func} {}

			void OnEvent(Event& event) { m_event_handler(event); }
		private:
			EventHandler m_event_handler;
		};

	}
//#endif

}