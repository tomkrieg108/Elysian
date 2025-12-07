#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/kernal/application.h"
#include "elysian/renderer/opengl_renderer.h"
//#include "elysian/camera/perspective_camera.h"
//#include "elysian/camera/perspective_camera_controller.h"
#include "elysian/light/directional_light.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/scene/native_scripts/rotate_and_orbit.h"
#include "scene.h"

#include <glm/glm.hpp>
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
			m_registry.destroy(entity); //implicit conversion to entt::entity - see entity.h
		}

		//this is to try out with the viewport in the editor
		Entity Scene::CreateQuadEntity2(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			auto& transform_comp = entity.GetComponent<TransformComponent>();
			transform_comp.SetTransform(transform);
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetQuadMesh());
			//entity.AddComponent<ShaderHandleComponent>(*(ShaderRepo::Get("basic_colored_ub")));
			Entity camera_entity = FindEntityByName("Main Camera"s); //TODO - this will break if name is chaned in editor

			//NOTE  this is based on info in Anton's book about mouse picking
			decltype(auto) event_handler = [entity, camera_entity, this](Event& event) mutable
				{
					EventMouseButtonPressed* e = dynamic_cast<EventMouseButtonPressed*>(&event);
					if (e == nullptr)
						return;

					CORE_WARN("Scene::CreateQuadEntity2 - event data  (x,y): ({},{})", e->x, e->y);
					CORE_WARN("Scene::CreateQuadEntity2 - viewport size (x,y): ({},{})", this->m_viewport_size.x, this->m_viewport_size.y);

					auto& transform = (glm::mat4&)entity.GetComponent<TransformComponent>();
					auto& window = Application::GetInstance().GetWindow();
					
					//position of mouse click (in e) *should* be relative to the main viewport
					float x = (2.0f * float(e->x)) / this->m_viewport_size.x - 1.0f;
					float y = 1.0f - (2.0f * float(e->y)) / this->m_viewport_size.y;
					float z = 1.0f;

					CORE_WARN("Scene::CreateQuadEntity2 - Ray NDC (z,y,z): ({},{},{})", x, y, z);

					//auto& camera = (PerspectiveCamera&)camera_entity.GetComponent<PerspectiveCameraComponent>();
					auto& camera = (Camera&)camera_entity.GetComponent<CameraComponent>();
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

		Entity Scene::CreateGridEntity()
		{
			Entity entity = CreateEntity("Grid");
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetGridMesh(), ShaderRepo::Get("basic_lines_colored_ub"));
			return entity;
		}

		Entity Scene::CreateCameraEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			entity.GetComponent<TransformComponent>().SetTransform(transform);
			entity.AddComponent<CameraComponent>();
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetCubeMesh(), ShaderRepo::Get("basic_diffuse_ub"));
			return entity;
		}

		Entity Scene::CreateBoxEntity() {
			Entity entity = CreateEntity("Box");
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetCubeMesh(), ShaderRepo::Get("basic_diffuse_ub"));
			return entity;
		}

		Entity Scene::CreateOrbitingCubeEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);
			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			entity.GetComponent<TransformComponent>().SetTransform(transform);
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetCubeMesh(), ShaderRepo::Get("basic_specular_ub"));
			entity.AddComponent<NativeScriptableComponent>().Bind<NativeScriptRotateAndOrbit>();
			return entity;
		}

		Entity Scene::CreateQuadEntity(const glm::vec3& position, const std::string& name)
		{
			Entity entity = CreateEntity(name);

			glm::mat4 transform = glm::translate(glm::mat4{ 1.0f }, position);
			entity.GetComponent<TransformComponent>().SetTransform(transform);
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetQuadMesh(), ShaderRepo::Get("basic_colored_ub"));

			decltype(auto) event_handler = [entity, this](Event& event) mutable
				{
					EventMouseButtonPressed* e = dynamic_cast<EventMouseButtonPressed*>(&event);
					if (e == nullptr)
						return;

					//Entity camera_entity = m_controlled_camera_entity;

					auto& transform = (glm::mat4&)entity.GetComponent<TransformComponent>();
					//glm::mat4& tr = entity.GetComponent<TransformComponent>(); //using implicit cast fn

					auto& window = Application::GetInstance().GetWindow();
					float x = (2.0f * float(e->x)) / (float)window.BufferWidth() - 1.0f;
					float y = 1.0f - (2.0f * float(e->y)) / (float)window.BufferHeight();
					float z = 1.0f;

					CORE_TRACE("Quad Entity click:");
					CORE_TRACE("Ray NDC (z,y,x): ({},{},{})", x, y, z);

					//auto& camera = (PerspectiveCamera&)m_controlled_camera_entity.GetComponent<PerspectiveCameraComponent>();
					auto& camera = (Camera&)m_controlled_camera_entity.GetComponent<CameraComponent>();
					auto& camera_transform = (glm::mat4&)m_controlled_camera_entity.GetComponent<TransformComponent>();

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
			entity.GetComponent<TransformComponent>().SetTransform(transform);
			entity.AddComponent<MeshRendererComponent>(MeshPrimitive::GetCubeMesh(), ShaderRepo::Get("white_ub"));
			entity.AddComponent<DirectionalLightComponent>();
			return entity;
		}
		
		//======================================================================================================================

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

		void Scene::BeginScene(const glm::vec4& clear_color)
		{
			OpenGLRenderer::SetLineWidth(2.0);
			OpenGLRenderer::SetClearColor(clear_color);
			OpenGLRenderer::ClearBuffers();

			auto& camera = (Camera&)(m_controlled_camera_entity.GetComponent<CameraComponent>());
			auto& camera_transform = (glm::mat4&)(m_controlled_camera_entity.GetComponent<TransformComponent>());

			auto view = m_registry.view<DirectionalLightComponent>();
			ASSERT(view.size() == 1); //only 1 dir light currently supported

			const auto& entity = view.front();
			DirectionalLight& light = m_registry.get<DirectionalLightComponent>(entity);
			glm::mat4& light_transform = m_registry.get<TransformComponent>(entity);
			light.direction = glm::normalize(glm::vec3(light_transform[2])); //i.e. pos z-dir of light (in world space)
			OpenGLRenderer::Begin(camera, light, camera_transform, light_transform);
		}

		void Scene::EndScene()
		{
		//TODO	- this shoud call destroy on all the scriptable components
		}

		//TODO:  this should be OnScenePlay()
		//call OnDesctroy in OnSceneStop()
		void Scene::UpdateScene(double time_step)
		{
			auto view = m_registry.view<NativeScriptableComponent>();
			for (auto entity : view)
			{
				auto& script_comp = view.get<NativeScriptableComponent>(entity);
				if (!script_comp.m_instance)
				{
					script_comp.m_instance = script_comp.InstantiateScript();
					script_comp.m_instance->m_entity = { entity, &m_registry };
					script_comp.m_instance->OnCreate();
				}
				script_comp.m_instance->OnUpdate(time_step);
			}
		}

		void Scene::RenderScene()
		{
			auto view = m_registry.view<TagComponent, TransformComponent, MeshRendererComponent>();

			for (auto entity : view)
			{
				auto [tag_comp, transform_comp, mesh_renderer_comp] = view.get<TagComponent, TransformComponent, MeshRendererComponent>(entity);

				if (!mesh_renderer_comp.enable_render)
					continue;

				const Material material = mesh_renderer_comp.material;
				const auto shader_ref = material.GetShader();
				const Mesh mesh = mesh_renderer_comp.mesh;

				OpenGLRenderer::Draw(mesh, material, (glm::mat4&)(transform_comp), *shader_ref);

				if (mesh_renderer_comp.show_coords)
				{
					auto& coords_mesh = MeshPrimitive::GetCoordSystemMesh();
					auto coords_shader_ref = ShaderRepo::Get("basic_lines_colored_ub");
					Material coords_mat = Material{ coords_shader_ref };

					glm::mat4 transform = transform_comp;
					if (tag_comp.m_tag != "Grid"s) //TODO - breaks if tag changed in editor
						transform = glm::scale(transform, glm::vec3(0.1f));

					OpenGLRenderer::Draw(coords_mesh, coords_mat,transform, *coords_shader_ref);
				}
			}
		}
#
		bool Scene::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
		{
			bool alt_pressed = Input::IsKeyPressed(GLFW_KEY_LEFT_ALT) || Input::IsKeyPressed(GLFW_KEY_RIGHT_ALT);
			if (alt_pressed || (e.btn != GLFW_MOUSE_BUTTON_LEFT))
				return true;
		
			auto view = m_registry.view<EventHandlerComponent>();
			for (auto entity : view)
			{
				auto& comp = view.get<EventHandlerComponent>(entity);
				comp.OnEvent(e);
			}
			return true;
		}

		bool Scene::OnWindowResize(ely::EventWidowResize& e)
		{
			if ((e.buffer_width == 0) || (e.buffer_height == 0))
				return true; //minimized

			auto view = m_registry.view<CameraComponent>();
			for (auto camera_entity : view)
			{
				Camera& camera = view.get<CameraComponent>(camera_entity);
				camera.SetAspectRatio((float)e.buffer_width, (float)e.buffer_height);
			}
			return true;
		}

		bool Scene::OnViewportResize(ely::EventViewportResize& e)
		{
			if ((e.width == 0) || (e.height == 0))
				return true; //minimized

			auto view = m_registry.view<CameraComponent>();
			for (auto camera_entity : view)
			{
				Camera& camera = view.get<CameraComponent>(camera_entity);
				camera.SetAspectRatio((float)e.width, (float)e.height);
			}
			return true;
		}

		void Scene::SetControlledCameraEntity(Entity camera_entity)
		{
			m_camera_controller.SetCameraEntity(camera_entity);
			m_controlled_camera_entity = camera_entity;
		}

		void Scene::SetRenderable(Entity& entity, bool val)
		{
			if (entity.HasComponent<MeshRendererComponent>())
			{
				auto& mesh_comp = entity.GetComponent<MeshRendererComponent>();
				mesh_comp.enable_render = val;
			}
		}

		void Scene::DisplayCoords(Entity& entity, bool val)
		{
			if (entity.HasComponent<MeshRendererComponent>())
			{
				auto& mesh_comp = entity.GetComponent<MeshRendererComponent>();
				mesh_comp.show_coords = val;
			}
		}
}