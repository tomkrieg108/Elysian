#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/application.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/camera/perspective_camera_controller.h"
#include "elysian/light/directional_light.h"
#include "elysian/model/mesh_primitives.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ely {

		Scene::Scene()
		{
		}

		Scene::~Scene()
		{
		}

		Entity Scene::CreateEntity(const std::string& name)
		{
			Entity entity = Entity(&m_registry);
			auto& id = entity.AddComponent<IDComponent>();
			auto& tag = entity.AddComponent<TagComponent>(name);
			auto& transform = entity.AddComponent<TransformComponent>();

			UUID uuid = static_cast<UUID>(entity.GetComponent<IDComponent>());
			m_entity_map[uuid] = entity;
			return entity;
		}

		void Scene::DestroyEntity(Entity entity)
		{
			m_entity_map.erase(entity.GetUUID());
			m_registry.destroy(entity);
		}

		Entity Scene::CreateGridEntity()
		{
			Entity entity = CreateEntity("Grid");
			entity.AddComponent<MeshComponent>(MeshPrimitive::GetGridMesh(20.0f));
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("coords")));
			return entity;
		}

		Entity Scene::CreateCoordSysEntity()
		{
			Entity entity = CreateEntity("Coord System");
			entity.AddComponent<MeshComponent>(MeshPrimitive::GetCoordSystemMesh(20.0f));
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("coords")));
			return entity;
		}

		Entity Scene::CreatePerspectiveCameraEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			entity.GetComponent<TransformComponent>().SetTransform(transform);
			Mesh mesh = MeshPrimitive::GetCubeMesh1();
			mesh.SetMaterial(*MaterialRepo::Get("empty")); //TODO make this a parameter to ctr
			auto& mesh_comp = entity.AddComponent<MeshComponent>(mesh);
			mesh_comp.SetEnableRender(false); //TODO make this a parameter to ctr
			auto& camera_comp = entity.AddComponent<PerspectiveCameraComponent>();
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("white")));
			return entity;
		}

		Entity Scene::CreateOrbitingCubeEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);

			auto& transform_comp = entity.GetComponent<TransformComponent>();
			transform_comp.SetTransform(transform);
			entity.AddComponent<MeshComponent>(MeshPrimitive::GetCubeMesh1());
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("light_map_diff_spec")));
			
			decltype(auto) update_func = [entity](double time_step) mutable 
			{
				//NOTE - captured variables are const by default - use the mutable keyword.  capture by reference caused crash
				//NOTE - didn't work when passed the transform_comp. had to pass entity
				auto& transform_comp = entity.GetComponent<TransformComponent>();
				glm::mat4& transform = (glm::mat4&)transform_comp;
				//rotate cube
				glm::vec3 rot_axis = glm::vec3(1, 1, 1);
				float angle = (float)time_step; // 1/60 radians 
				transform = glm::rotate(transform, angle, rot_axis);
				//orbit box about world y axis
				glm::mat4 orbit_axis = glm::mat4(1.0f);
				orbit_axis = glm::rotate(orbit_axis, angle, glm::vec3(0, 1, 0));
				transform = orbit_axis * transform;
			};
			entity.AddComponent<UpdatableComponent>(update_func);
			return entity;
		}

		Entity Scene::CreateQuadEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			auto& transform_comp = entity.GetComponent<TransformComponent>();
			transform_comp.SetTransform(transform);
			entity.AddComponent<MeshComponent>(MeshPrimitive::GetQuadMesh1());
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("colored_basic")));
			Entity camera_entity = FindEntityByName("Main Camera"s);

			decltype(auto) event_handler = [entity, camera_entity](Event& event) mutable
			{
					EventMouseButtonPressed* e = dynamic_cast<EventMouseButtonPressed*>(&event);
					if (e == nullptr)
						return;

					auto& transform = (glm::mat4&)entity.GetComponent<TransformComponent>();

					auto& window = Application::GetInstance().GetWindow();
					float x = (2.0f * float(e->x)) / window.BufferWidth() - 1.0f;
					float y = 1.0f - (2.0f * float(e->y)) / window.BufferHeight();
					float z = 1.0f;

					auto& camera = (PerspectiveCamera&)camera_entity.GetComponent<PerspectiveCameraComponent>();
					auto& camera_transform = (glm::mat4&)camera_entity.GetComponent<TransformComponent>();

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
			};
			entity.AddComponent<EventHandlerComponent>(event_handler);
			return entity;
		}
		
		Entity Scene::CreateDrirectionalLightEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, position);
			transform = glm::scale(transform, glm::vec3(0.4f));
			auto& transform_comp = entity.GetComponent<TransformComponent>();
			transform_comp.SetTransform(transform);
			auto& mesh_comp = entity.AddComponent<MeshComponent>(MeshPrimitive::GetCubeMesh1());
			mesh_comp.m_mesh.SetMaterial(*MaterialRepo::Get("empty"));
			entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("white")));
			entity.AddComponent<DirectionalLightComponent>();
			return entity;
		}

		Entity Scene::FindEntityByName(std::string_view name)
		{
			auto view = m_registry.view<TagComponent>();
			for(auto entity_handle : view)
			{
				const TagComponent& comp = view.get<TagComponent>(entity_handle);
				std::string tag = (std::string)comp;
				if (tag == name)
					return Entity{ entity_handle, &m_registry };
			}
			return Entity{ entt::null, &m_registry };
		}

		Entity Scene::GetEntityByUUID(UUID uuid)
		{
			//TODO assert exists.  How do I use this!!??
			entt::entity entity_handle = m_entity_map.at(uuid); //read only, include range check, throws std::out_of_range
			Entity entity = Entity(entity_handle, &m_registry);
			return entity;
		}

		void Scene::UploadCameraDataToShaders(const std::string& camera_name)
		{
			auto camera_entity = FindEntityByName(camera_name);
			//TODO ASSERT VALID
			//TODO  how to find by ID?  need to save the ID of commonly acessed components?
			auto& camera = (PerspectiveCamera)(camera_entity.GetComponent<PerspectiveCameraComponent>());
			auto& camera_transform = (glm::mat4)(camera_entity.GetComponent<TransformComponent>());
			glm::mat4 view_mat = camera.GetViewMatrix(camera_transform);
			glm::mat4 proj_mat = camera.GetProjMatrix();

			//upload camera data to all shaders!
			for (auto& item : ShaderRepo::GetShaders())
			{
				auto& shader = item.second;
				shader->Bind();
				shader->SetUniformMat4f("u_view", view_mat);
				shader->SetUniformMat4f("u_proj", proj_mat);
			}

			//TODO - should be using the component shader
			auto cube_shader = ely::ShaderRepo::Get("light_map_diff_spec");
			cube_shader->Bind();
			cube_shader->SetUniform3f("u_view_pos", camera.GetPosition(camera_transform));
		}

		void Scene::UploadLightDataToShader()
		{
			auto light_entity = FindEntityByName("Directional Light");
			//TODO - should be using the component shader
			auto cube_shader = ely::ShaderRepo::Get("light_map_diff_spec");
			cube_shader->Bind();

			auto& light = (DirectionalLight)light_entity.GetComponent<DirectionalLightComponent>();
			light.UploadDataToShader(cube_shader);
		}
		
		void Scene::BeginScene(const std::string& camera_name, const glm::vec4& clear_color)
		{
			//Upload per scene data do shader(s)
			OpenGLRenderer::SetLineWidth(1.0);
			UploadCameraDataToShaders(camera_name);
			UploadLightDataToShader();
			OpenGLRenderer::SetClearColor(clear_color);
			OpenGLRenderer::ClearBuffers();
		}

		void Scene::BeginScene(const std::string& camera_name, const OpenGLFramebuffer& framebuffer, const glm::vec4& clear_color)
		{
			framebuffer.Bind();
			OpenGLRenderer::SetLineWidth(2.0);
			UploadCameraDataToShaders(camera_name);
			UploadLightDataToShader();
			OpenGLRenderer::SetClearColor(clear_color);
			OpenGLRenderer::ClearBuffers();
		}

		void Scene::EndScene()
		{
			
		}

		void Scene::UpdateScene(double time_step)
		{
			auto view = m_registry.view<UpdatableComponent>();
			for (auto entity : view)
			{
				auto& updatable_comp = view.get<UpdatableComponent>(entity);
				updatable_comp.OnUpdate(time_step);
			}
		}

		void Scene::OnEvent(Event& event)
		{
			EventWidowResize* e = dynamic_cast<EventWidowResize*>(&event);
			if (e != nullptr)
			{
				m_camera_controller.OnWindowResize(*e);
				return;
			}
			auto view = m_registry.view<EventHandlerComponent>();

			for (auto entity : view)
			{
				auto& comp = view.get<EventHandlerComponent>(entity);
				comp.OnEvent(event);
			}
		}

		void Scene::RenderScene()
		{
			//auto group = m_registry.group<TransformComponent, MeshComponent, ShaderHandleComponent>(); //crashes
			auto view = m_registry.view<TagComponent, TransformComponent, MeshComponent, ShaderHandleComponent>();

			for (auto entity : view)
			{
				auto& [tag_comp, transform_comp, mesh_comp, shader_comp] = view.get<TagComponent, TransformComponent, MeshComponent, ShaderHandleComponent>(entity);

				auto& shader = (Shader&)(shader_comp.GetShader());
				shader.Bind();
				shader.SetUniformMat4f("u_model", (glm::mat4)(transform_comp));

				if (!mesh_comp.GetEnableRender())
					continue;

				auto& mesh = (Mesh)(mesh_comp);
				mesh.UploadMaterialToShader(shader);
				OpenGLRenderer::DrawMesh(mesh, shader);

				if (tag_comp.m_tag == "Coord System"s)
					continue;

				if (mesh_comp.GetShowCoords())
				{
					auto& coords_entity = FindEntityByName("Coord System"s);
					auto& coords_mesh = (Mesh&)coords_entity.GetComponent<MeshComponent>();
					auto& coords_shader = (Shader&)(coords_entity.GetComponent<ShaderHandleComponent>().GetShader());
					glm::mat4 transform = transform_comp;
					transform =	glm::scale(transform, glm::vec3(0.1f));
					coords_shader.Bind();
					coords_shader.SetUniformMat4f("u_model", transform);
					OpenGLRenderer::DrawMesh(coords_mesh, coords_shader);
				}
			}
		} 

		void Scene::SetControlledCameraEntity(Entity camera_entity)
		{
			m_camera_controller.SetCameraEntity(camera_entity);
			m_controlled_camera_entity = camera_entity;
		}

		void Scene::SetRenderable(Entity& entity, bool val)
		{
			//TODO - use asserts?
			if (!entity)
				return;
			if(entity.HasComponent<MeshComponent>())
			{
				auto& mesh_comp = entity.GetComponent<MeshComponent>();
				mesh_comp.SetEnableRender(val);
			}
		}
		void Scene::DisplayCoords(Entity& entity, bool val)
		{
			//TODO - use asserts?
			if (!entity)
				return;
			if (entity.HasComponent<MeshComponent>())
			{
				auto& mesh_comp = entity.GetComponent<MeshComponent>();
				mesh_comp.SetShowCoords(val);
			}
		}



		/*
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
					transform{ transform }
				{}

				operator glm::mat4& () { return transform; }
				operator const glm::mat4& () const { return transform; }

				glm::mat4 transform;
			};


			entt::entity entity = m_registry.create(); //return val is uint32_t

			auto& transform = m_registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
			m_registry.emplace<MeshComponent>(entity);

			//m_registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>; //compile error

			//if (m_registry.has<TransformComponent>(entity))) //compile error has()
			{
				TransformComponent& component = m_registry.get<TransformComponent>(entity);
			}

			auto view = m_registry.view<TransformComponent>();  //get a list of all the transform components for all entities
			for (auto entity : view)
			{
				//view.get<TransformComponent>(entity); //alternative to below
				TransformComponent& component = m_registry.get<TransformComponent>(entity);
			}

			auto group = m_registry.group<TransformComponent>(entt::get < MeshComponent>);

			for (auto entity : group)
			{
				auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			}
		}
		*/


}