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

		for (auto ent : registry.view<entt::entity>()) 
		{
			Entity entity{ ent , &registry };
			DrawEntityNode(entity);
		}
		ImGui::End();

		/*if (ImGui::IsWindowHovered && ImGui::IsMouseDown(0))
		{
			m_selected_entity = {};
		}*/

		//--------------------------------------------------

		ImGui::Begin("Properties");
		if (m_selected_entity)
		{
			DrawComponents(m_selected_entity);
		}
		ImGui::End();
	}

	void SceneHeirachyPanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)753737374346, flags, tag.c_str()); //(uint64_t) has to be non-const in entity class for this to work?
	
		if (ImGui::IsItemClicked()) {
			m_selected_entity = entity;
		}
			
		if (opened)
		{
			bool opened = ImGui::TreeNodeEx((void *)(uint64_t)753737374347, flags, tag.c_str());
			if(opened) 
				ImGui::TreePop();
			ImGui::TreePop();
		}


	}

	void SceneHeirachyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>();

			static char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				glm::mat4& mat = entity.GetComponent<TransformComponent>();
				ImGui::DragFloat3("position", glm::value_ptr(mat[3]), 0.1, 0, 0);
				ImGui::TreePop();
			}
			
		}
	}

}