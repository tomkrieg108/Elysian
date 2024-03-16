#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/model/mesh.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/renderer/opengl_draw_mode.h"
#include "elysian/scene/entity.h"
#include "elysian/kernal/uuid.h"

#include "test_layer3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

#include <cmath>

//TODO:  want to set this up so that there are no references to any openGL calls

TestLayer3::TestLayer3(ely::Window& window) :
	m_window{ window },
	m_framebuffer(window.BufferWidth(), window.BufferHeight()),
	m_framebuffer_alt(window.BufferWidth(), window.BufferHeight())
{

	//TODO - this event stuff shouldn't be in the client?
	ely::EventDispatcher::SetCallback(this, &TestLayer3::OnKeyPressed);
	ely::EventDispatcher::SetCallback(this, &TestLayer3::OnMouseMoved);
	ely::EventDispatcher::SetCallback(this, &TestLayer3::OnMouseScrolled);
	ely::EventDispatcher::SetCallback(this, &TestLayer3::OnWindowResize);
	ely::EventDispatcher::SetCallback(this, &TestLayer3::OnMouseButtonPressed);

	m_scene.Init();
}


void TestLayer3::OnAttach()
{
}

void TestLayer3::OnDetach()
{
}

void TestLayer3::OnUpdate(double time_step)
{
	m_scene.GetCameraController()->OnUpdate(time_step);
	auto camera_main_entity = m_scene.GetEntity("camera_main_entity");
	m_scene.Begin(camera_main_entity);
	//-----------------------------------------------------------------------------------
	//Render to screen (main camera)
	//-----------------------------------------------------------------------------------
	
	//draw the loaded model
	//m_model_shader->Bind();
	//m_model_shader->SetUniformMat4f("projection", m_camera_controller.GetCamera().GetProjMatrix());
	//m_model_shader->SetUniformMat4f("view", m_camera_controller.GetCamera().GetViewMatrix());
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	//m_model_shader->SetUniformMat4f("model", model);
	//m_model->Draw(*m_model_shader);

	m_scene.OnUpdate(time_step);
	m_scene.Render();
	m_scene.End();

	//-----------------------------------------------------------------------------------
	//Render to framebuffer (main camera)
	//-----------------------------------------------------------------------------------

	m_scene.Begin(camera_main_entity, m_framebuffer); //this is going to reset the same view and proj mat in all the shaders!
	m_scene.Render();
	m_scene.End();
	m_framebuffer.Unbind();

	//------------------------------------------------------------------------------------------
	//render to framebuffer alt
	//------------------------------------------------------------------------------------------
	auto camera_alt_entity = m_scene.GetEntity("camera_alt_entity");
	auto camera_main_coords_entity = m_scene.GetEntity("camera_main_coords_entity");
	auto camera_alt_coords_entity = m_scene.GetEntity("camera_alt_coords_entity");
	camera_alt_entity->RemoveMeshComponent();
	camera_main_entity->AddMeshComponent();
	m_scene.Begin(camera_alt_entity, m_framebuffer_alt);
	m_scene.Render();
	m_scene.End();
	m_framebuffer_alt.Unbind();
	camera_alt_entity->AddMeshComponent();
	camera_main_entity->RemoveMeshComponent();
}

void TestLayer3::OnImGuiRender()
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


void TestLayer3::OnKeyPressed(ely::EventKeyPressed& e)
{
	if (e.key == GLFW_KEY_ESCAPE)
		m_window.ShutDown();
	else if (e.key == GLFW_KEY_SPACE)
		m_window.ToggleCursorEnabled();

	else if (e.key == GLFW_KEY_T)
	{
		if (m_scene.GetCameraController()->GetCameraEntity()->GetName() == "camera_main_entity"s)
			m_scene.GetCameraController()->SetCameraEntity(m_scene.GetEntity("camera_alt_entity"s));
		else
			m_scene.GetCameraController()->SetCameraEntity(m_scene.GetEntity("camera_main_entity"s));
	}
}

void TestLayer3::OnMouseMoved(ely::EventMouseMoved& e)
{
	if (!m_window.GetCursorEnabled())
	{
		m_scene.GetCameraController()->OnMouseMoved(e);
	}
}

void TestLayer3::OnMouseScrolled(ely::EventMouseScrolled& e)
{
	m_scene.GetCameraController()->OnMouseScrolled(e);
}

void TestLayer3::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
{
	m_scene.OnEvent(e);
}

void TestLayer3::OnWindowResize(ely::EventWidowResize& e)
{
}

