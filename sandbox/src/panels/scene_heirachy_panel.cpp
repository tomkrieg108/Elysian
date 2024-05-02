#include "pch.h"
#include "elysian/scene/scene.h"

#include "elysian/scene/component.h"
#include "panels/scene_heirachy_panel.h"

#include <entt/entt.hpp>
#include <imgui_docking/imgui.h>
#include <imgui_docking/imgui_internal.h>
#include <imgui_docking/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace ely {

	//If a new entity is selected, use this to defer rendering components of the new selection until next frame.
	//Prevents a 'quirk' in ImGui::InputText which causes the newly selected entity to take on the value of the tag from the previously
	//selected entity if the text input control was still active when the new selection was made
	static bool s_selected_entity_changed = false;

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
		
		ImGui::Begin("Scene Heirachy");

		if (m_scene)
		{
			for (auto ent : registry.view<entt::entity>())
			{
				Entity entity{ ent , &registry };
				DrawEntityNode(entity);
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			{
				m_selected_entity = {};
				s_selected_entity_changed = false;
			}
				
			//Right click on blank space in scene heirachy window
			ImGuiPopupFlags flags = ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems;
			if (ImGui::BeginPopupContextWindow(0, flags))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_scene->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}
		}
		
		ImGui::End();

		//--------------------------------------------------

		ImGui::Begin("Properties");
		
		if (m_selected_entity && !s_selected_entity_changed)
		{
			DrawComponents(m_selected_entity);

			//button to create new entity
			//TODO - only add component if doesn't already have it

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
		s_selected_entity_changed = false;
	}

	void SceneHeirachyPanel::DrawEntityNode(Entity entity)
	{
		auto id = (uint64_t)(UUID&)entity.GetComponent<IDComponent>();;
		std::string& tag = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		bool opened = ImGui::TreeNodeEx((void*)id, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			s_selected_entity_changed = (entity != m_selected_entity);
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
			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)753737374347, flags, tag.c_str());
			if (opened)
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

	//might wan't additional ui function(s)
	template <typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, bool allow_remove, UIFunction ui_function)
	{
		const ImGuiTreeNodeFlags treenode_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 content_region_available = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treenode_flags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(content_region_available.x - line_height * 0.5f);
			if (ImGui::Button("+", ImVec2{ line_height, line_height }))
				ImGui::OpenPopup("ComponentSettings");

			bool remove_component = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (allow_remove)
				{
					if (ImGui::MenuItem("Remove Component"))
						remove_component = true;
				}
				ImGui::EndPopup();
			}

			if (open)
			{
				ui_function(component);
				ImGui::TreePop();
			}

			if (remove_component)
				entity.RemoveComponent<T>();
		}
	}

	
	void SceneHeirachyPanel::DrawComponents(Entity entity)
	{
		// Tag ----------------------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			std::string& tag = entity.GetComponent<TagComponent>();

			static char buffer[256] = { "\0" };
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));

			//returns true only when the buffer changes
			//If still active / has focus, then a different entity is selected from the entity list, the function changes buffer to current contents
			//of text field (i.e the previously selected entity).  Hence the need for s_selected_entity_changed
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}

			//if (ImGui::IsItemActive())
			//	ImGui::Text("Active");
			
			ImGui::PopStyleVar();
		}

		ImGui::Spacing();

		// Transform -----------------------------------------------------------

		DrawComponent<TransformComponent>("Transform", entity, false, [](auto& component) {
			//TODO - static_assert for component type
			glm::mat4& transform = (glm::mat4&)(component);
			ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f, 0, 0);
		});


		DrawComponent<PerspectiveCameraComponent>("Perspective Camera", entity, true, [](auto& component) {
			ImGui::Text("Perspective Camera");
		});

		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, true, [](auto& component) {
			ImGui::Text("Mesh");
		});

	}

}