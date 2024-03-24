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
		EventHandlerComponent(EventHandler func) : m_event_handler{func} {}

		void OnEvent(Event& event) { m_event_handler(event); }
	private:
		EventHandler m_event_handler;
	};

}