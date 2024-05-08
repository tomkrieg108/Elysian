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

//#include "elysian/scene/scriptable_entity.h" //NOTE - get circular #includes with this.  need forward declaration of NativeScriptComponent
#include <type_traits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace std::literals;

namespace ely
{
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
		operator std::string& () { return m_tag; }
		operator const std::string& () const { return m_tag; }
	public:
		std::string m_tag{ "Unnamed Entity"s };
	};

	//---------------------------------------------------------------

	class TransformComponent
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent& component) = default;
		TransformComponent(const glm::mat4& transform) : m_transform{transform} {}

		void SetTransform(const glm::mat4& transform) { m_transform = transform; }

		glm::vec3& GetTranslation()
		{
			return (glm::vec3&)(m_transform[3]); //& seems to be optional!
		}

		operator glm::mat4& () { return m_transform; }
		operator const glm::mat4& () const { return m_transform; }

		glm::mat4 MakeTransform()
		{
			glm::mat4 ident_mat{ 1.0f };
			glm::mat4 rot_mat = glm::toMat4(glm::quat(glm::radians(m_rotation)));
			glm::mat4 scale_mat = glm::scale(ident_mat, m_scale);
			//glm::mat4 translateion_mat = glm::translate(ident_mat, m_translation);
			glm::mat4 translateion_mat = glm::translate(ident_mat, GetTranslation());
			m_transform = translateion_mat * rot_mat * scale_mat;
			return m_transform;
		}

		glm::vec3 GetEulerAnglesInDegrees()
		{
			//glm::quat q = glm::quat(m_rotation);
			glm::quat quaternion = glm::quat_cast(m_transform);
			m_rotation = glm::eulerAngles(quaternion);
			return glm::degrees(m_rotation);
		}

	public:
		glm::mat4 m_transform = glm::mat4(1.0f);

		//TODO - maybe better to store as position, rotation, scale components seperately for use in editor
		//reconstruct matrix when any of them are modified
		//this is what Cherno does
		glm::vec3 m_translation{ 0.0f,0.0f,0.0f }; //not currently used - extracted from m_transform
		glm::vec3 m_scale{ 1.0f,1.0f,1.0f };
		glm::vec3 m_rotation{ 0.0f,0.0f,0.0f }; //Euler angles in degrees

		glm::quat q = glm::quat(m_rotation); // (0,0,0,1)
		//glm::quat q_ident = glm::quat_identity(); //unit quaternian with zero rotation - should be the same as above
		glm::quat q_conj = glm::conjugate(q); //(-0, -0, -0, 1);
		int a = 1;
	};

	//---------------------------------------------------------------
	struct MeshComponent
	{
		MeshComponent(const Ref<Shader>& shader) :
			m_material(shader)
		{
		}
		Mesh m_mesh;
		material_v2::Material m_material;
	};

	//---------------------------------------------------------------

	class MeshRendererComponent
	{
		//TODO should perhaps make sure move constructors are availabe too
	public:
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent& component) = default;
		MeshRendererComponent(const Mesh& mesh) : m_mesh{ mesh } {}

		void SetEnableRender(bool val) { m_enable_render = val; }
		bool GetEnableRender() const { return m_enable_render; }

		void SetShowCoords(bool val) { m_show_coords = val; }
		bool GetShowCoords() const { return m_show_coords; }

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
		operator ShaderHandle () const { return m_shader_handle;}
	public:
		ShaderHandle m_shader_handle;
	};

	//---------------------------------------------------------------
	//TODO - obsolete!
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

	//---------------------------------------------------------------
	class ScriptableEntity;
	class NativeScriptableComponent
	{
	public:
		template <typename T>
		void Bind()
		{
			static_assert(std::is_base_of_v<ScriptableEntity,T>);
			InstantiateScript = []() {return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](ScriptableEntity* scriptable_entity) {delete scriptable_entity;  scriptable_entity = nullptr; };
		}

	public:
		//NOTE: could instead use std::function, but apparently it has quite a lot of overhead
		//NOTE - can assign function pointer only to lambdas that don't capture - see above
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(ScriptableEntity*);
		ScriptableEntity* m_instance = nullptr; //Defer instantiation of this until scene starts running
	};

	//-----------------------------------------------------------------------------

	//TODO - can this be done as a NativeScriptableComponent
	using EventHandler = std::function<void(Event&)>;
	class EventHandlerComponent
	{
	public:
		EventHandlerComponent() = default;
		EventHandlerComponent(EventHandler func) : m_event_handler{ func } {}
		void OnEvent(Event& event) { m_event_handler(event); }
	public:
		EventHandler m_event_handler;
	};

}