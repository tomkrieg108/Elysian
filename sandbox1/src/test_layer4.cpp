#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/application.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/model/mesh.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/renderer/opengl_draw_mode.h"
#include "elysian/scene/entity.h"
#include "elysian/scene/scene.h"
#include "elysian/kernal/uuid.h"

#include "test_layer4.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

//TODO:  want to set this up so that there are no references to any openGL calls

TestLayer4::TestLayer4() :
	m_window{ ely::Application::GetInstance().GetWindow()},
	m_framebuffer(m_window.BufferWidth(), m_window.BufferHeight()),
	m_framebuffer_alt(m_window.BufferWidth(), m_window.BufferHeight())
{
	//TODO - this event stuff shouldn't be in the client?
	ely::EventDispatcher::SetCallback(this, &TestLayer4::OnKeyPressed);
	ely::EventDispatcher::SetCallback(this, &TestLayer4::OnMouseMoved);
	ely::EventDispatcher::SetCallback(this, &TestLayer4::OnMouseScrolled);
	ely::EventDispatcher::SetCallback(this, &TestLayer4::OnWindowResize);
	ely::EventDispatcher::SetCallback(this, &TestLayer4::OnMouseButtonPressed);
}


void TestLayer4::OnAttach()
{
	m_scene.CreateGridEntity();
	m_scene.CreateCoordSysEntity();
	m_scene.CreateDrirectionalLightEntity(glm::vec3(1.2f, 1.0f, 2.0f), "Directional Light"s);
	m_scene.CreateOrbitingCubeEntity(glm::vec3(-2.0f, 2.0f, 3.0f), "Orbiting Cube"s);
	
	auto& main_camera_entity = m_scene.CreatePerspectiveCameraEntity(glm::vec3(0.0f, 3.0f, 15.0f), "Main Camera"s);
	m_scene.SetRenderable(main_camera_entity, false);
	m_scene.SetControlledCameraEntity(main_camera_entity);

	auto& alt_camera_entity = m_scene.CreatePerspectiveCameraEntity(glm::vec3(4.0, 1.0, 7.0), "Alt Camera"s);
	auto& mesh_comp = alt_camera_entity.GetComponent<ely::scene_v2::MeshComponent>();
	m_scene.SetRenderable(alt_camera_entity, true);

	m_scene.CreateQuadEntity(glm::vec3(2.0f, 0.0, 4.0f), "Yellow Quad"s); //NOTE: relies on main camera=> need to create main cam first
}

void TestLayer4::OnDetach()
{
}

void TestLayer4::OnUpdate(double time_step)
{
	m_scene.GetCameraController().OnUpdate(time_step);

	//render to screen
	glm::vec4 clear_color{ 0.1f,0.1f,0.1f,1.0f };
	m_scene.BeginScene("Main Camera"s, clear_color);
	m_scene.UpdateScene(time_step);
	m_scene.RenderScene();
	m_scene.EndScene();

	//-----------------------------------------------------------------------------------
	//Render to framebuffer (main camera)
	//-----------------------------------------------------------------------------------
	m_scene.BeginScene("Main Camera"s, m_framebuffer); //this is going to reset the same view and proj mat in all the shaders!
	m_scene.RenderScene();
	m_scene.EndScene();
	m_framebuffer.Unbind();

	//-----------------------------------------------------------------------------------
	//Render to framebuffer (Alt camera)
	//-----------------------------------------------------------------------------------
	
	auto& main_camera_entity = m_scene.FindEntityByName("Main Camera"s);
	auto& alt_camera_entity = m_scene.FindEntityByName("Alt Camera"s);
	auto& main_mesh_comp = main_camera_entity.GetComponent<ely::scene_v2::MeshComponent>();
	auto& alt_mesh_comp = alt_camera_entity.GetComponent<ely::scene_v2::MeshComponent>();
	main_mesh_comp.SetEnableRender(true);
	alt_mesh_comp.SetEnableRender(false);

	m_scene.BeginScene("Alt Camera"s, m_framebuffer_alt); //this is going to reset the same view and proj mat in all the shaders!
	m_scene.RenderScene();
	m_scene.EndScene();
	m_framebuffer_alt.Unbind();

	main_mesh_comp.SetEnableRender(false);
	alt_mesh_comp.SetEnableRender(true);
}

void TestLayer4::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Width, Height %d %d : ", m_window.Width(), m_window.Height());
		ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window.BufferWidth(), m_window.BufferHeight());
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("Light Properties"))
	{
		//ImGui::SliderFloat3("Light Pos ", &(m_light_pos[0]), -5.0f, 5.0f);
		//ImGui::SliderFloat3("Ambient ", &(m_light_ambient[0]), 0.0f, 1.0f);
		//ImGui::SliderFloat3("Diffuse ", &(m_light_diffuse[0]), 0.0f, 1.0f);
		//ImGui::SliderFloat3("Specular ", &(m_light_specular[0]), 0.0f, 1.0f);
	}
	if (ImGui::CollapsingHeader("Box Properties"))
	{
		//ImGui::SliderFloat3("Box Pos ", &(m_cube_pos[0]), -5.0f, 5.0f);
		//ImGui::SliderFloat("Shininess ", &(m_cube_shininess), 1.0f, 256.0f);
	}
	if (ImGui::CollapsingHeader("Framebuffer - main camera"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t color_attachment_id = (uint64_t)m_framebuffer.GetColourAttachmentID(); //uint64_t to stop compiler warning
		ImTextureID tex_id = (void*)color_attachment_id;
		ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Framebuffer - alt camera"))
	{
		float tex_height = 400.0;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t color_attachment_id = (uint64_t)m_framebuffer_alt.GetColourAttachmentID();
		ImTextureID tex_id = (void*)color_attachment_id;
		ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
}


void TestLayer4::OnKeyPressed(ely::EventKeyPressed& e)
{
	if (e.key == GLFW_KEY_ESCAPE)
		m_window.ShutDown();
	else if (e.key == GLFW_KEY_SPACE)
		m_window.ToggleCursorEnabled();

	else if (e.key == GLFW_KEY_T)
	{
		auto& main_camera_entity = m_scene.FindEntityByName("Main Camera"s);
		auto& alt_camera_entity = m_scene.FindEntityByName("Alt Camera"s);

		ely::scene_v2::Entity controlled_camera_entity = m_scene.GetControlledCameraEntity();
		auto& tag = (std::string&)controlled_camera_entity.GetComponent<ely::scene_v2::TagComponent>();
		if (tag == "Main Camera"s)
			m_scene.SetControlledCameraEntity(alt_camera_entity);
		else
			m_scene.SetControlledCameraEntity(main_camera_entity);
	}
}

void TestLayer4::OnMouseMoved(ely::EventMouseMoved& e)
{
	if (!m_window.GetCursorEnabled())
	{
		m_scene.GetCameraController().OnMouseMoved(e);
	}
}

void TestLayer4::OnMouseScrolled(ely::EventMouseScrolled& e)
{
	m_scene.GetCameraController().OnMouseScrolled(e);
}

void TestLayer4::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
{
	m_scene.OnEvent(e);
}

void TestLayer4::OnWindowResize(ely::EventWidowResize& e)
{
}


