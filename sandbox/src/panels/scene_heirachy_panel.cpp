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
					//m_selected_entity.AddComponent<PerspectiveCameraComponent>();
					m_selected_entity.AddComponent<CameraComponent>();
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
			if (allow_remove)
			{
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component"))
						remove_component = true;
					ImGui::EndPopup();
				}
			}

			if (open)
			{
				ui_function(component);
				ImGui::TreePop();
			}

			if (remove_component)
				entity.RemoveComponent<T>();

			//ImGui::Spacing(); ImGui::Spacing();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+8.0f);
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		//Thankyou Cherno!
		bool updated = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			updated = true;
		}
			
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		updated |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			updated = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		updated |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			updated = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		updated |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return updated;
	}

	void SceneHeirachyPanel::DrawComponents(Entity entity)
	{
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
				tag = std::string(buffer);
			
			ImGui::PopStyleVar();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
		}
		
		DrawComponent<TransformComponent>("Transform", entity, false, [](auto& component) {
			//TODO - static_assert for component type
			//glm::mat4& transform = (glm::mat4&)(component);
			//auto& pos_vec = (glm::vec3&)(*glm::value_ptr(transform[3])); //This works.  & seems to be optional!
			bool transform_updated = false;
			transform_updated |= DrawVec3Control("Translation", component.GetTranslation()); //extracted from transform matrix
			//transform_updated |= DrawVec3Control("Rotation (Deg)", component.GetEulerAnglesInDegrees()); //extracted from transform matrix
			transform_updated |= DrawVec3Control("Rotation (Deg)", component.m_rotation);
			transform_updated |= DrawVec3Control("Scale", component.m_scale, 1.0f);

			if (transform_updated)
				component.MakeTransform();
		});

		DrawComponent<CameraComponent>("Camera", entity, true, [](auto& component) {

			Camera& camera = (CameraComponent&)(component);

			const char* proj_strings[] = {"Perspective", "Orthographic"};
			const char* projection_string = (camera.GetProjectionType() == Camera::ProjectionType::Perspective) ? proj_strings[0] : proj_strings[1];
			//const char* projection_string = proj_strings[(int)(camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", projection_string))
			{
				for (int i = 0; i < 2; i++)
				{
					bool selected = (projection_string = proj_strings[i]);
					if (ImGui::Selectable(proj_strings[i], selected))
					{
						projection_string = proj_strings[i];
						camera.SetProjectionType((Camera::ProjectionType)i);
					};
					
					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
			{
				auto& perspective_params = camera.PerspectiveParameters();
				ImGui::DragFloat("Near", &(perspective_params.z_near), 0.2f, 0.01f, 1.0f, "%.2f");
				ImGui::DragFloat("Far", &(perspective_params.z_far), 1.0f, 100.0f, 10000.0f, "%.1f");
				ImGui::DragFloat("Vert FOV", &(perspective_params.fov), 0.2f, 15.0f, 75.0f, "%.1f");
				ImGui::Text("Aspect Ratio: %.2f", perspective_params.aspect_ratio);
			}
			else
			{
				auto& ortho_params = camera.OrthoParameters();
				ImGui::DragFloat("Left", &(ortho_params.left), 0.2f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("Right", &(ortho_params.right), 0.2f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("Top", &(ortho_params.top), 0.2f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("Bottom", &(ortho_params.bottom), 0.2f, -100.0f, 100.0f, "%.1f");
				ImGui::DragFloat("Near", &(ortho_params.z_near), 0.2f, 0.01f, 1.0f, "%.2f");
				ImGui::DragFloat("Far", &(ortho_params.z_far), 1.0f, 100.0f, 10000.0f, "%.1f");
			}
		
		});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, true, [](auto& component) {

			DirectionalLight& light = (DirectionalLight&)(component);
			
			ImGui::ColorEdit3("Ambient##2f", (float*)&light.ambient_color, ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Diffuse##2f", (float*)&light.color, ImGuiColorEditFlags_Float);
		
		});

		DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, true, [](auto& component) {
			ImGui::Text("Mesh");
		});

	}

}