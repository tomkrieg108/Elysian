#include "pch.h"
#include "elysian/scene/scene.h"

#include "elysian/scene/component.h"
#include "panels/scene_heirachy_panel.h"

#include <entt/entt.hpp>
#include <imgui_docking/imgui.h>

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

		//ImGui::Begin("Properties");
		//ImGui::End();
	}

	void SceneHeirachyPanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (m_selected_entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, tag.c_str());
	
		if (ImGui::IsItemClicked()) {
			m_selected_entity = entity;
		}
			
		if (opened)
		{
			bool opened = ImGui::TreeNodeEx((void *)753737374347, flags, tag.c_str());
			if(opened) 
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}


}