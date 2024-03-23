#include "pch.h"
#include "elysian/renderer/opengl_renderer.h"
#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>


namespace ely {

//#if (SCENE_VERS == 1)
	inline namespace scene_v1 {

		void Entity::Render()
		{
			if (m_components.find("mesh") == m_components.end())
				return; //no mesh components => not renderable

			const auto& mesh = std::dynamic_pointer_cast<MeshComponent>(m_components["mesh"])->GetMesh();
			const auto& shader = std::dynamic_pointer_cast<ShaderComponent>(m_components["shader"])->GetShader();

			if (m_components.find("light") != m_components.end())
				m_components["light"]->UploadToShader(ely::ShaderRepo::Get("light_map_diff_spec"));

			m_components["transform"]->UploadToShader(shader);
			OpenGLRenderer::DrawMesh(mesh, shader); //this uploads material data too
		}

		void Entity::OnUpdate(double time_step)
		{
			if (m_components.find("behaviour") != m_components.end())
			{
				const auto behaviour_comp = std::dynamic_pointer_cast<BehaviourComponent>(m_components["behaviour"]);
				behaviour_comp->OnUpdate(time_step);
			}
		}

		void Entity::OnEvent(Event& event)
		{
			if (m_components.find("event_handler") != m_components.end())
			{
				const auto event_comp = std::dynamic_pointer_cast<EventHandlerComponent>(m_components["event_handler"]);
				event_comp->OnEvent(event);
			}
		}
	}
//#endif

}

namespace ely {
	
//#if (SCENE_VERS == 2)
	namespace scene_v2 {
		static IDComponent component = IDComponent();
	}

//#endif

}