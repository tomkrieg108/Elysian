#include "pch.h"
#include "elysian/scene/scene.h"

#include "elysian/scene/component.h"
#include "panels/scene_heirachy_panel.h"

#include <entt/entt.hpp>
#include <imgui_docking/imgui.h>

#include <glm/gtc/type_ptr.hpp>

namespace ely {

	SceneHeirachyPanel::SceneHeirachyPanel(Ref<Scene>& scene) : 
		m_scene {scene}
	{
		//TODO assert scene not null
	}

	void SceneHeirachyPanel::SetScene(Ref<Scene>& scene)
	{
		//TODO assert scene not null
		m_scene = scene;
	}
	void SceneHeirachyPanel::OnImGuiRender()
	{
		auto& registry = m_scene->GetRegistry();
		auto* regp = &registry;

		ImGui::Begin("Scene Heirachy");

		if (m_scene)
		{
			for (auto ent : registry.view<entt::entity>())
			{
				Entity entity{ ent , &registry };
				DrawEntityNode(entity);
			}
			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
				m_selected_entity = {};
		}
		
		//Right click on blank space in scene heirachy window
		if (ImGui::BeginPopupContextWindow(0,1))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_scene->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}
		
		ImGui::End();

		//--------------------------------------------------

		ImGui::Begin("Properties");
		if (m_selected_entity)
		{
			DrawComponents(m_selected_entity);

			//button to create new entity

			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent"); //AddComponent is the id for the popup
			
			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_selected_entity.AddComponent<PerspectiveCameraComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Mesh Renderer"))
				{
					m_selected_entity.AddComponent<MeshRendererComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

		}
		ImGui::End();
	}

	void SceneHeirachyPanel::DrawEntityNode(Entity entity)
	{
		auto id = (int64_t)(UUID&)entity.GetComponent<IDComponent>();;
		std::string& tag = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		bool opened = ImGui::TreeNodeEx((void*)id, flags, tag.c_str());
	
		if (ImGui::IsItemClicked()) 
		{
			m_selected_entity = entity;
		}

		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entity_deleted = true;

			ImGui::EndPopup();
		}
			
		if (opened)
		{
			//to create nested entities
			bool opened = ImGui::TreeNodeEx((void *)(uint64_t)753737374347, flags, tag.c_str());
			if(opened) 
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entity_deleted)
		{
			m_scene->DestroyEntity(entity);
			if (m_selected_entity == entity)
				m_selected_entity = {};
		}
			
	}

	void SceneHeirachyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			std::string& tag = entity.GetComponent<TagComponent>();

			static char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
			ImGui::PopStyleVar();
		}

		const ImGuiTreeNodeFlags treenode_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (entity.HasComponent<TransformComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treenode_flags, "Transform");
			ImGui::SameLine(ImGui::GetWindowWidth() - 30.0f);
			if (ImGui::Button("+", ImVec2{ 30.0f, 30.0f }))
				ImGui::OpenPopup("ComponentSettings");
			ImGui::PopStyleVar();
		
			bool remove_component = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					remove_component = true;
				
				ImGui::EndPopup();
			}

			if (open)
			{
				glm::mat4& mat = entity.GetComponent<TransformComponent>();
				ImGui::DragFloat3("position", glm::value_ptr(mat[3]), 0.1f, 0, 0);
				ImGui::TreePop();
			}

			//Shouln't be able to remove transform component
			//if (remove_component)
			//	entity.RemoveComponent<TransformComponent>();
		}

		if (entity.HasComponent<PerspectiveCameraComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treenode_flags, "Perspective Camera");
			ImGui::SameLine(ImGui::GetWindowWidth() - 30.0f);
			if (ImGui::Button("+", ImVec2{ 30.0f, 30.0f }))
				ImGui::OpenPopup("ComponentSettings");
			ImGui::PopStyleVar();

			bool remove_component = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					remove_component = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				PerspectiveCamera& camera = entity.GetComponent<PerspectiveCameraComponent>();
				ImGui::Text("Perspective Camera");
				ImGui::TreePop();
			}

			if (remove_component)
				entity.RemoveComponent<TransformComponent>();
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treenode_flags, "Mesh Renderer");
			ImGui::SameLine(ImGui::GetWindowWidth() - 30.0f);
			if (ImGui::Button("+", ImVec2{ 30.0f, 30.0f }))
				ImGui::OpenPopup("ComponentSettings");
			ImGui::PopStyleVar();

			bool remove_component = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					remove_component = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				Mesh& mesh_renderer = entity.GetComponent<MeshRendererComponent>();
				ImGui::Text("Mesh");
				ImGui::TreePop();
			}

			if (remove_component)
				entity.RemoveComponent<TransformComponent>();
		}

	}

}