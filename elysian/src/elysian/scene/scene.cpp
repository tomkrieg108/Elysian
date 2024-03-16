#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/application.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/light/directional_light.h"
#include "elysian/model/mesh_primitives.h"
#include "entity.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ely
{
#if (SCENE_VERS == 1)

		void Scene::Init()
		{
			//---------------------------------------------------------------------------------
			//Grid
			//---------------------------------------------------------------------------------
			auto grid_entity = CreateRef<Entity>("grid_entity");
			auto grid_tag_comp = CreateRef<TagComponent>(*grid_entity, "grid_comp"s);
			auto grid_transform_comp = CreateRef<TransformComponent>(*grid_entity);
			auto grid_mesh_comp = CreateRef<MeshComponent>(*grid_entity, MeshPrimitive::GetGridMesh(20.0f, 1.0f));
			auto grid_shader_comp = CreateRef<ShaderComponent>(*grid_entity, ShaderRepo::Get("coords"));
			grid_entity->AddComponent(grid_tag_comp);
			grid_entity->AddComponent(grid_transform_comp);
			grid_entity->AddComponent(grid_mesh_comp);
			grid_entity->AddComponent(grid_shader_comp);
			m_entities.push_back(grid_entity);
			//---------------------------------------------------------------------------------
			//Grid Coords
			//---------------------------------------------------------------------------------
			auto grid_coords_entity = CreateRef<Entity>("grid_coords_entity");
			auto grid_coords_tag_comp = CreateRef<TagComponent>(*grid_coords_entity, "grid_coords_comp"s);
			auto grid_coords_mesh_comp = CreateRef<MeshComponent>(*grid_coords_entity,
				MeshPrimitive::GetCoordSystemMesh(grid_transform_comp->GetTransform(), 20.0f));

			grid_coords_entity->AddComponent(grid_coords_tag_comp);
			grid_coords_entity->AddComponent(grid_transform_comp); //same transform comp as for grid
			grid_coords_entity->AddComponent(grid_coords_mesh_comp);
			grid_coords_entity->AddComponent(grid_shader_comp); //same shader as for grid
			m_entities.push_back(grid_coords_entity);
			//---------------------------------------------------------------------------------
			//Orbiting cube
			//---------------------------------------------------------------------------------
			auto cube_entity = CreateRef<Entity>("cube_entity");
			auto cube_tag_comp = CreateRef<TagComponent>(*cube_entity, "cube_comp"s);
			auto cube_transform = glm::mat4(1.0f);
			cube_transform = glm::translate(cube_transform, glm::vec3(-2.0f, 2.0f, 3.0f));
			auto cube_transform_comp = CreateRef<TransformComponent>(*cube_entity, cube_transform);
			auto cube_mesh_comp = CreateRef< MeshComponent>(*cube_entity, MeshPrimitive::GetCubeMesh());
			auto cube_shader_comp = CreateRef<ShaderComponent>(*cube_entity, ShaderRepo::Get("light_map_diff_spec"));
			auto cube_behaviour_comp = CreateRef<BehaviourComponent>(*cube_entity, [cube_transform_comp](double time_step) {
				auto& transform = cube_transform_comp->GetTransform();
				//rotate cube
				glm::vec3 rot_axis = glm::vec3(1, 1, 1);
				float angle = (float)time_step; // 1/60 radians 
				transform = glm::rotate(transform, angle, rot_axis);
				//orbit box about world y axis
				glm::mat4 orbit_axis = glm::mat4(1.0f);
				orbit_axis = glm::rotate(orbit_axis, angle, glm::vec3(0, 1, 0));
				transform = orbit_axis * transform;
				});
			cube_entity->AddComponent(cube_tag_comp);
			cube_entity->AddComponent(cube_transform_comp);
			cube_entity->AddComponent(cube_mesh_comp);
			cube_entity->AddComponent(cube_shader_comp);
			cube_entity->AddComponent(cube_behaviour_comp);
			m_entities.push_back(cube_entity);
			//---------------------------------------------------------------------------------
			//Orbiting cube coords
			//---------------------------------------------------------------------------------
			auto cube_coords_entity = CreateRef<Entity>("cube_coords_entity");
			auto cube_coords_tag_comp = CreateRef<TagComponent>(*cube_coords_entity, "cube_coords_comp"s);
			auto cube_coords_mesh_comp = CreateRef<MeshComponent>(*cube_coords_entity,
				MeshPrimitive::GetCoordSystemMesh(grid_transform_comp->GetTransform(), 2.0f));

			cube_coords_entity->AddComponent(cube_coords_tag_comp);
			cube_coords_entity->AddComponent(cube_transform_comp); //same transform comp as for orbiting cube
			cube_coords_entity->AddComponent(cube_coords_mesh_comp);
			cube_coords_entity->AddComponent(grid_shader_comp); //same shader as for grid
			m_entities.push_back(cube_coords_entity);
			//---------------------------------------------------------------------------------
			// Light Box
			//---------------------------------------------------------------------------------
			auto light_box_entity = CreateRef<Entity>("light_box_entity");
			auto light_box_tag_comp = CreateRef<TagComponent>(*light_box_entity, "light_box_comp"s);
			auto light_transform = glm::mat4(1.0f);
			light_transform = glm::translate(light_transform, glm::vec3(1.2f, 1.0f, 2.0f));
			light_transform = glm::scale(light_transform, glm::vec3(0.2f)); // a smaller cube
			auto light_transform_comp = CreateRef<TransformComponent>(*light_box_entity, light_transform);

			auto light_mesh = MeshPrimitive::GetCubeMesh();
			light_mesh->SetMaterial(*MaterialRepo::Get("empty"));
			auto light_mesh_comp = CreateRef< MeshComponent>(*light_box_entity, light_mesh);
			auto light_shader_comp = CreateRef<ShaderComponent>(*light_box_entity, ShaderRepo::Get("white"));
			auto light_comp = CreateRef<LightComponent>(*light_box_entity, DirectionalLight());

			light_box_entity->AddComponent(light_box_tag_comp);
			light_box_entity->AddComponent(light_transform_comp);
			light_box_entity->AddComponent(light_mesh_comp);
			light_box_entity->AddComponent(light_shader_comp);
			light_box_entity->AddComponent(light_comp);
			m_entities.push_back(light_box_entity);

			//////////////////////////////////////////////////////////////////////////////////
			// Cameras and controllers - TODO - need to give this some more thought!!
			//////////////////////////////////////////////////////////////////////////////////

			//---------------------------------------------------------------------------------
			// Alt Camera
			//---------------------------------------------------------------------------------
			auto camera_alt_entity = CreateRef<Entity>("camera_alt_entity");
			auto camera_alt_tag_comp = CreateRef<TagComponent>(*camera_alt_entity, "camera_alt_comp"s);
			auto camera_alt_transform = glm::mat4(1.0f); //TODO camera already contains a transform matrix
			camera_alt_transform = glm::translate(camera_alt_transform, glm::vec3(4.0, 1.0, 7.0));
			auto camera_alt_transform_comp = CreateRef<TransformComponent>(*camera_alt_entity, camera_alt_transform);
			auto camera_alt_mesh = MeshPrimitive::GetCubeMesh();
			camera_alt_mesh->SetMaterial(*MaterialRepo::Get("empty"));
			auto camera_alt_mesh_comp = CreateRef< MeshComponent>(*camera_alt_entity, camera_alt_mesh);
			auto camera_alt_shader_comp = CreateRef<ShaderComponent>(*camera_alt_entity, ShaderRepo::Get("white"));
			PerspectiveCamera* camera_alt = new PerspectiveCamera();
			auto camera_alt_comp = CreateRef< PerspectiveCameraComponent>(*camera_alt_entity, *camera_alt);
			camera_alt_entity->AddComponent(camera_alt_tag_comp);
			camera_alt_entity->AddComponent(camera_alt_transform_comp);
			camera_alt_entity->AddComponent(camera_alt_mesh_comp);
			camera_alt_entity->AddComponent(camera_alt_shader_comp);
			camera_alt_entity->AddComponent(camera_alt_comp);
			m_entities.push_back(camera_alt_entity);

			//---------------------------------------------------------------------------------
			// Alt Camera coords
			//---------------------------------------------------------------------------------
			auto camera_alt_coords_entity = CreateRef<Entity>("camera_alt_coords_entity");
			auto camera_alt_coords_tag_comp = CreateRef<TagComponent>(*camera_alt_coords_entity, "camera_alt_coords_comp"s);
			auto camera_alt_coords_mesh_comp = CreateRef<MeshComponent>(*camera_alt_coords_entity,
				MeshPrimitive::GetCoordSystemMesh(grid_transform_comp->GetTransform(), 2.0f));
			camera_alt_coords_entity->AddComponent(camera_alt_coords_tag_comp);
			camera_alt_coords_entity->AddComponent(camera_alt_transform_comp); //same transform as for camera alt
			camera_alt_coords_entity->AddComponent(camera_alt_coords_mesh_comp);
			camera_alt_coords_entity->AddComponent(grid_shader_comp); //same shader as for grid
			m_entities.push_back(camera_alt_coords_entity);


			//---------------------------------------------------------------------------------
			// Main Camera
			//---------------------------------------------------------------------------------
			auto camera_main_entity = CreateRef<Entity>("camera_main_entity");
			auto camera_main_tag_comp = CreateRef<TagComponent>(*camera_main_entity, "camera_main_comp"s);
			auto camera_main_transform = glm::mat4(1.0f);
			camera_main_transform = glm::translate(camera_alt_transform, glm::vec3(0.0f, 3.0f, 15.0f));
			auto camera_main_transform_comp = CreateRef<TransformComponent>(*camera_main_entity, camera_main_transform);
			auto camera_main_mesh = MeshPrimitive::GetCubeMesh();
			camera_main_mesh->SetMaterial(*MaterialRepo::Get("empty"));
			auto camera_main_mesh_comp = CreateRef< MeshComponent>(*camera_main_entity, camera_main_mesh);
			auto camera_main_shader_comp = CreateRef<ShaderComponent>(*camera_main_entity, ShaderRepo::Get("white"));

			PerspectiveCamera* camera_main = new PerspectiveCamera();
			auto camera_main_comp = CreateRef< PerspectiveCameraComponent>(*camera_main_entity, *camera_main);
			camera_main_entity->AddComponent(camera_main_tag_comp);
			camera_main_entity->AddComponent(camera_main_transform_comp);
			//camera_main_entity->AddComponent(camera_main_mesh_comp);
			camera_main_entity->AddComponent(camera_main_shader_comp);
			camera_main_entity->AddComponent(camera_main_comp);
			m_entities.push_back(camera_main_entity);

			//---------------------------------------------------------------------------------
			// Main Camera coords
			//---------------------------------------------------------------------------------
			auto camera_main_coords_entity = CreateRef<Entity>("camera_main_coords_entity");
			auto camera_main_coords_tag_comp = CreateRef<TagComponent>(*camera_main_coords_entity, "camera_main_coords_comp"s);
			auto camera_main_coords_mesh_comp = CreateRef<MeshComponent>(*camera_main_coords_entity,
				MeshPrimitive::GetCoordSystemMesh(grid_transform_comp->GetTransform(), 2.0f));
			camera_main_coords_entity->AddComponent(camera_main_coords_tag_comp);
			camera_main_coords_entity->AddComponent(camera_main_transform_comp); //same transform as for camera main
			camera_main_coords_entity->AddComponent(camera_main_coords_mesh_comp);
			camera_main_coords_entity->AddComponent(grid_shader_comp); //same shader as for grid
			m_entities.push_back(camera_main_coords_entity);

			//---------------------------------------------------------------------------------
			// Yellow Quad that moved when mouse clocked
			//---------------------------------------------------------------------------------
			auto quad_entity = CreateRef<Entity>("quad_entity");
			auto quad_tag_comp = CreateRef<TagComponent>(*quad_entity, "quad_comp"s);
			auto quad_transform = glm::mat4(1.0f);
			quad_transform = glm::translate(quad_transform, glm::vec3(2.0f, 0.0, 4.0f));
			auto quad_transform_comp = CreateRef<TransformComponent>(*quad_entity, quad_transform);
			auto quad_mesh_comp = CreateRef<MeshComponent>(*quad_entity, MeshPrimitive::GetQuadMesh());
			auto quad_shader_comp = CreateRef<ShaderComponent>(*quad_entity, ely::ShaderRepo::Get("colored_basic"));
			auto quad_event_handler_comp = CreateRef<EventHandlerComponent>(*quad_entity, [quad_transform_comp, this](Event& event) {

				EventMouseButtonPressed* e = dynamic_cast<EventMouseButtonPressed*>(&event);
				if (e == nullptr)
					return;

				auto& transform = quad_transform_comp->GetTransform();

				//----------------------------------------------
				auto& window = Application::GetInstance().GetWindow();
				float x = (2.0f * float(e->x)) / window.BufferWidth() - 1.0f;
				float y = 1.0f - (2.0f * float(e->y)) / window.BufferHeight();
				float z = 1.0f;

				auto camera_entity = this->GetEntity("camera_main_entity");
				auto& camera = std::dynamic_pointer_cast<PerspectiveCameraComponent>(camera_entity->GetComponent("camera"))->GetCamera();
				auto& camera_transform = std::dynamic_pointer_cast<TransformComponent>(camera_entity->GetComponent("transform"))->GetTransform();

				glm::vec3 ray_nds = glm::vec3(x, y, z);
				glm::vec4 ray_clip = glm::vec4(x, y, -1, 1);
				glm::vec4 ray_eye = camera.GetInverseProjMatrix() * ray_clip;

				ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
				glm::vec4 ray_world_4d = camera_transform * ray_eye; //transform mat is the inverse of the view mat


				glm::vec3 ray_world = glm::normalize(glm::vec3(ray_world_4d[0], ray_world_4d[1], ray_world_4d[2]));

				glm::vec3 eye_world = glm::vec3(camera_transform[3][0], camera_transform[3][1], camera_transform[3][2]);
				glm::vec3 grid_normal = glm::vec3(0, 1, 0);
				float t = -glm::dot(grid_normal, eye_world) / glm::dot(grid_normal, ray_world);
				glm::vec3 intersection = eye_world + t * ray_world;

				transform[3][0] = floorf(intersection.x);
				transform[3][1] = 0.0f;
				transform[3][2] = floorf(intersection.z);

				//---------------------------------- 	

				});

			quad_entity->AddComponent(quad_tag_comp);
			quad_entity->AddComponent(quad_transform_comp);
			quad_entity->AddComponent(quad_mesh_comp);
			quad_entity->AddComponent(quad_shader_comp);
			quad_entity->AddComponent(quad_tag_comp);
			quad_entity->AddComponent(quad_event_handler_comp);
			m_entities.push_back(quad_entity);

			m_camera_controller = new PerspectiveCameraController(camera_main_entity);
		}

		void Scene::Begin(Ref<Entity> camera_entity, const glm::vec4& clear_color)
		{
			OpenGLRenderer::SetLineWidth(1.0);
			LoadCameraData(camera_entity);
			ely::OpenGLRenderer::SetClearColor(clear_color);
			ely::OpenGLRenderer::ClearBuffers();
		}

		void Scene::Begin(Ref<Entity> camera_entity, const OpenGLFramebuffer& framebuffer, const glm::vec4& clear_color)
		{
			framebuffer.Bind();
			OpenGLRenderer::SetLineWidth(2.0);
			LoadCameraData(camera_entity);
			ely::OpenGLRenderer::SetClearColor(clear_color);
			ely::OpenGLRenderer::ClearBuffers();
		}

		void Scene::LoadCameraData(Ref<Entity> camera_entity)
		{
			auto& components = camera_entity->GetComponents();
			auto& camera_comp = std::dynamic_pointer_cast<PerspectiveCameraComponent>(components["camera"]);
			//TODO assert not null
			auto& camera = camera_comp->GetCamera();

			auto& transform_comp = std::dynamic_pointer_cast<TransformComponent>(components["transform"]);
			//TODO assert not null
			auto& camera_transform = transform_comp->GetTransform();

			glm::mat4 view_mat = camera.GetViewMatrix(camera_transform);
			glm::mat4 proj_mat = camera.GetProjMatrix();

			for (auto& item : ShaderRepo::GetShaders())
			{
				auto& shader = item.second;
				shader->Bind();
				shader->SetUniformMat4f("u_view", view_mat);
				shader->SetUniformMat4f("u_proj", proj_mat);
			}

			auto cube_shader = ely::ShaderRepo::Get("light_map_diff_spec");
			cube_shader->Bind();
			cube_shader->SetUniform3f("u_view_pos", camera.GetPosition(camera_transform));
		}

		Ref<Entity> Scene::GetEntity(const std::string& name)
		{
			for (auto& entity : m_entities)
			{
				if (entity->GetName() == name)
					return entity;
			}
			return nullptr;
		}

		void Scene::OnUpdate(double time_step)
		{
			for (const auto& entity : m_entities)
				entity->OnUpdate(time_step);
		}

		void Scene::OnEvent(Event& event)
		{
			EventWidowResize* e = dynamic_cast<EventWidowResize*>(&event);
			if (e != nullptr)
			{
				m_camera_controller->OnWindowResize(*e);
				return;
			}


			for (const auto& entity : m_entities)
				entity->OnEvent(event);
		}

		void Scene::Render()
		{
			for (const auto& entity : m_entities)
				entity->Render();
		}
#endif
}


namespace ely {

#if (SCENE_VERS == 2)
	//from cherno ch77 game series

	static void OnTransformConstruct(entt::registry& reg, entt::entity ent)
	{

	}

	Scene::Scene()
	{
		//glm::value_ptr with imgui

		struct MeshComponent 
		{
			float data;
		};

		struct TransformComponent
		{
			TransformComponent() = default;
			TransformComponent(const glm::mat4& transform) :
				transform{transform}
			{}

			operator glm::mat4& () { return transform; }
			operator const glm::mat4& () const { return transform; }
	
			glm::mat4 transform;
		};


		entt::entity entity = m_registry.create(); //return val is uint32_t

		auto& transform = m_registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

		//m_registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>; //compile error

		//if (m_registry.has<TransformComponent>(entity))) //compile error has()
		{
			TransformComponent& component = m_registry.get<TransformComponent>(entity);
		}

		auto view = m_registry.view<TransformComponent>();  //get a list of all the transform components for all entities
		for (auto entity : view)
		{
			view.get<TransformComponent>(entity); //alternative to below
			TransformComponent& component = m_registry.get<TransformComponent>(entity);
		}

		auto group = m_registry.group<TransformComponent>(entt::get < MeshComponent>);

		for (auto entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity); 
		}
	}

	Scene::~Scene()
	{

	}

#endif

}