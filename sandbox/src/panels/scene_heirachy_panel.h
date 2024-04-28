#pragma once
#include "elysian/kernal/base.h"
	
namespace ely {

	class Scene;

	class SceneHeirachyPanel
	{
	public:
		SceneHeirachyPanel() = default;
		SceneHeirachyPanel(Ref<Scene>& scene);
		void SetScene(Ref<Scene>& scene);
		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

	private:
		Ref<Scene> m_scene;
		Entity m_selected_entity;
	};


}