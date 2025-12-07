#include "pch.h" //TODO - not using pch?
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/kernal/uuid.h"
#include "elysian/kernal/application.h"
#include "elysian/events/events.h"
#include "elysian/events/event_dispatcher.h"

#include "elysian/model/mesh.h"
#include "elysian/model/mesh_primitives.h"

#include "elysian/renderer/opengl_renderer.h"
#include "elysian/renderer/opengl_draw_mode.h"
#include "elysian/renderer/opengl_shader.h"

#include "elysian/scene/entity.h"
#include "elysian/scene/scene.h"

#include "test_layer4.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

namespace ely {

	TestLayer4::TestLayer4() :
		m_window{ Application::GetInstance().GetWindow() },
		m_framebuffer(m_window.BufferWidth(), m_window.BufferHeight()),
		m_framebuffer_alt(m_window.BufferWidth(), m_window.BufferHeight())
	{
	}

	void TestLayer4::OnAttach()
	{
		m_scene = ely::CreateRef<ely::Scene>();
		m_scene_heirachy_panel.SetScene(m_scene);

		m_scene->CreateGridEntity();
		m_scene->CreateDrirectionalLightEntity( glm::vec3(1.2f, 1.0f, 2.0f), "Directional Light 2"s );
		m_scene->CreateOrbitingCubeEntity(glm::vec3(-2.0f, 2.0f, 3.0f), "Orbiting Cube 2"s);
		m_main_camera_entity = m_scene->CreateCameraEntity(glm::vec3(-5.0f, 5.0f, 15.0f), "Main Camera 2"s);
		m_alt_camera_entity = m_scene->CreateCameraEntity(glm::vec3(4.0, 2.0, 7.0), "Alt Camera 2"s);
		m_scene->CreateQuadEntity(glm::vec3(2.0f, 0.0, 4.0f), "Yellow Quad 2"s);
		
		m_scene->SetRenderable(m_main_camera_entity, false); 
		m_scene->SetRenderable(m_alt_camera_entity, true);
		m_scene->SetControlledCameraEntity(m_main_camera_entity);

		m_scene->GetCameraController().LookAt(glm::vec3{ 0,0,0 });
	}

	void TestLayer4::OnDetach()
	{
	}

	void TestLayer4::OnUpdate(double time_step)
	{
		m_scene->GetCameraController().OnUpdate(time_step);

		//render to screen
		OpenGLRenderer::SetLineWidth(1.0);
		glm::vec4 clear_color{ 0.13f,0.13f,0.13f,1.0f };
		m_scene->BeginScene(clear_color); 
		m_scene->UpdateScene(time_step); //TODO call this when the play betton is pressed
		m_scene->RenderScene(); 
		m_scene->EndScene();

		//-----------------------------------------------------------------------------------
		//Render to framebuffer (main camera)
		//-----------------------------------------------------------------------------------
		OpenGLRenderer::SetLineWidth(2.0);
		m_framebuffer.Bind();
		m_scene->BeginScene(clear_color);
		m_scene->RenderScene();
		m_scene->EndScene();
		m_framebuffer.Unbind();

		//-----------------------------------------------------------------------------------
		//Render to framebuffer (Alt camera)
		//-----------------------------------------------------------------------------------
		/*m_scene->SetControlledCameraEntity(m_alt_camera_entity);
		m_scene->SetRenderable(m_alt_camera_entity, false);
		m_scene->SetRenderable(m_main_camera_entity, true);
		OpenGLRenderer::SetLineWidth(2.0);
		m_framebuffer_alt.Bind();
		m_scene->BeginScene(clear_color);
		m_scene->RenderScene();
		m_scene->EndScene();
		m_framebuffer_alt.Unbind();
		m_scene->SetControlledCameraEntity(m_main_camera_entity);
		m_scene->SetRenderable(m_alt_camera_entity, true);
		m_scene->SetRenderable(m_main_camera_entity, false);*/
	}

	void TestLayer4::OnEvent(ely::Event& e)
	{
		EventDispatcher dispatcher(e);

		/*switch (e.Type())
		{
			case EventType::KeyPressed: OnKeyPressed(dynamic_cast<EventKeyPressed&>(e)); return;
		}*/


		dispatcher.Dispatch<ely::EventKeyPressed>(std::bind(&TestLayer4::OnKeyPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<ely::EventMouseMoved>(std::bind(&TestLayer4::OnMouseMoved, this, std::placeholders::_1));
		dispatcher.Dispatch<ely::EventMouseScrolled>(std::bind(&TestLayer4::OnMouseScrolled, this, std::placeholders::_1));
		dispatcher.Dispatch<ely::EventMouseButtonPressed>(std::bind(&TestLayer4::OnMouseButtonPressed, this, std::placeholders::_1));

		//TODO - should be in application.cpp
		dispatcher.Dispatch<ely::EventWidowResize>(std::bind(&TestLayer4::OnWindowResize, this, std::placeholders::_1));
	}

	bool TestLayer4::OnKeyPressed(ely::EventKeyPressed& e)
	{
		//CORE_TRACE(" TestLayer4::OnKeyPressed() called: {}", e);

		if (e.key == GLFW_KEY_ESCAPE)
			ely::Application::GetInstance().Close();

		else if (e.key == GLFW_KEY_T)
		{
			if (m_scene->GetControlledCameraEntity() == m_main_camera_entity)
			{
				m_scene->SetControlledCameraEntity(m_alt_camera_entity);
				m_scene->SetRenderable(m_alt_camera_entity, false);
				m_scene->SetRenderable(m_main_camera_entity, true);
			}
			else
			{
				m_scene->SetControlledCameraEntity(m_main_camera_entity);
				m_scene->SetRenderable(m_alt_camera_entity, true);
				m_scene->SetRenderable(m_main_camera_entity, false);
			}
		}

		return true;
	}

	bool TestLayer4::OnMouseMoved(ely::EventMouseMoved& e)
	{
		m_scene->GetCameraController().OnMouseMoved(e);
		return true;
	}

	bool TestLayer4::OnMouseScrolled(ely::EventMouseScrolled& e)
	{
		//CORE_TRACE(" TestLayer4::OnMouseScrolled() called: {}", e);
		m_scene->GetCameraController().OnMouseScrolled(e);
		return true;
	}

	bool TestLayer4::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
	{
		//CORE_TRACE(" TestLayer4::OnMouseButtonPressed2() called: {},{}", e.x, e.y);
		m_scene->OnMouseButtonPressed(e);
		return true;
	}

	bool TestLayer4::OnWindowResize(ely::EventWidowResize& e)
	{
		//CORE_WARN(" TestLayer4::OnWindowResize() called: {}", e);
		m_scene->OnWindowResize(e);
		return true;
	}

	void TestLayer4::OnImGuiRender()
	{
		//TODO - all this up t'll show demo shoud be moved into panels

		ImGui::Begin("Info"); //start window

		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("Width, Height %d %d : ", m_window.Width(), m_window.Height());
			ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window.BufferWidth(), m_window.BufferHeight());
			ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		//if (ImGui::CollapsingHeader("Main Camera"))
		//{
		//	auto& main_camera_entity = m_scene->FindEntityByName("Main Camera"s);
		//	auto& camera = (ely::PerspectiveCamera&)(main_camera_entity.GetComponent<ely::PerspectiveCameraComponent>());
		//	ImGui::SliderFloat("Aspect Ratio", camera.AspectRatioPtr(), 0.2f, 6.0f);
		//}
		//if (ImGui::CollapsingHeader("Light Properties"))
		//{
		//	/*ImGui::SliderFloat3("Light Pos ", &(m_light_pos[0]), -5.0f, 5.0f);
		//	ImGui::SliderFloat3("Ambient ", &(m_light_ambient[0]), 0.0f, 1.0f);
		//	ImGui::SliderFloat3("Diffuse ", &(m_light_diffuse[0]), 0.0f, 1.0f);
		//	ImGui::SliderFloat3("Specular ", &(m_light_specular[0]), 0.0f, 1.0f);*/
		//}
		//if (ImGui::CollapsingHeader("Box Properties"))
		//{
		//	//ImGui::SliderFloat3("Box Pos ", &(m_cube_pos[0]), -5.0f, 5.0f);
		//	//ImGui::SliderFloat("Shininess ", &(m_cube_shininess), 1.0f, 256.0f);
		//}
		if (ImGui::CollapsingHeader("Framebuffer - main camera"))
		{
			float tex_height = 600.0f;
			float tex_width = tex_height * m_window.MonitorAspectRatio();
			uint64_t color_attachment_id = (uint64_t)m_framebuffer.GetColourAttachmentID(); //uint64_t to stop compiler warning
			ImTextureID tex_id = (void*)color_attachment_id;
			ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
		}
		if (ImGui::CollapsingHeader("Framebuffer - alt camera"))
		{
			float tex_height = 600.0f;
			float tex_width = tex_height * m_window.MonitorAspectRatio();
			//float tex_width = tex_height * m_window.AspectRatio();
			uint64_t color_attachment_id = (uint64_t)m_framebuffer_alt.GetColourAttachmentID();
			ImTextureID tex_id = (void*)color_attachment_id;
			ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
		}

		ImGui::End();

		//demo window
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		m_scene_heirachy_panel.OnImGuiRender();
	}

}