#include "pch.h" //TODO - not using pch?
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

//TODO - is all this needed?
#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_shader_utils.h"
#include "elysian/renderer/opengl_texture_2d.h"
#include "elysian/renderer/opengl_vertex_array.h"
#include "elysian/renderer/opengl_buffer.h"
#include "elysian/renderer/opengl_framebuffer.h"
#include "elysian/renderer/opengl_uniform_buffer.h"

#include "elysian/model/model_lgl.h"

#include "editor_layer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

//TODO:  want to set this up so that there are no references to any openGL calls

namespace ely {

	EditorLayer::EditorLayer() :
		m_window{ Application::GetInstance().GetWindow() },
		m_framebuffer(m_window.BufferWidth(), m_window.BufferHeight()),
		m_framebuffer_alt(m_window.BufferWidth(), m_window.BufferHeight())
	{
	}

	EditorLayer::~EditorLayer()
	{
	}
 
	void EditorLayer::OnAttach()
	{

		m_scene = CreateRef<Scene>();

		m_scene->CreateGridEntity();
		m_scene->CreateDrirectionalLightEntity(glm::vec3(1.2f, 1.0f, 2.0f), "Directional Light"s);
		m_scene->CreateOrbitingCubeEntity(glm::vec3(-2.0f, 2.0f, 3.0f), "Orbiting Cube"s);

		auto& main_camera_entity = m_scene->CreatePerspectiveCameraEntity(glm::vec3(0.0f, 3.0f, 15.0f), "Main Camera"s);
		m_scene->SetRenderable(main_camera_entity, false);
		m_scene->SetControlledCameraEntity(main_camera_entity);

		auto& alt_camera_entity = m_scene->CreatePerspectiveCameraEntity(glm::vec3(4.0, 1.0, 7.0), "Alt Camera"s);
		auto& mesh_comp = alt_camera_entity.GetComponent<ely::MeshRendererComponent>();
		m_scene->SetRenderable(alt_camera_entity, true);

		//uses viewport data
		m_scene->CreateQuadEntity2(glm::vec3(2.0f, 0.0, 4.0f), "Yellow Quad"s); //NOTE: relies on main camera=> need to create main cam first
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate(double time_step)
	{
		if(m_viewport_data.forcused && m_viewport_data.hovered)
			m_scene->GetCameraController().OnUpdate(time_step);

		//Render to framebuffer (main camera)
		ely::OpenGLRenderer::SetLineWidth(1.0);
		m_framebuffer.Bind();
		//m_scene->BeginScene("Main Camera"s, glm::vec4{ 0.1f,0.1f,0.1f,1.0f }); //this is going to reset the same view and proj mat in all the shaders!
		m_scene->BeginScene(glm::vec4{ 0.1f,0.1f,0.1f,1.0f }); //this is going to reset the same view and proj mat in all the shaders!
		m_scene->UpdateScene(time_step);
		m_scene->RenderScene();
		m_scene->EndScene();
		m_framebuffer.Unbind();

		
		//Render to framebuffer (Alt camera)
		//auto& main_camera_entity = m_scene->FindEntityByName("Main Camera"s);
		//auto& alt_camera_entity = m_scene->FindEntityByName("Alt Camera"s);
		//auto& main_mesh_comp = main_camera_entity.GetComponent<MeshRendererComponent>();
		//auto& alt_mesh_comp = alt_camera_entity.GetComponent<MeshRendererComponent>();
		//main_mesh_comp.SetEnableRender(true);
		//alt_mesh_comp.SetEnableRender(false);

		//m_framebuffer_alt.Bind();
		//m_scene->BeginScene("Alt Camera"s, glm::vec4{ 0.1f,0.1f,0.1f,1.0f }); //this is going to reset the same view and proj mat in all the shaders!
		//m_scene->RenderScene();
		//m_scene->EndScene();
		//m_framebuffer_alt.Unbind();

		//main_mesh_comp.SetEnableRender(false);
		//alt_mesh_comp.SetEnableRender(true);
	}

	
	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<EventKeyPressed>(std::bind(&EditorLayer::OnKeyPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<EventMouseMoved>(std::bind(&EditorLayer::OnMouseMoved, this, std::placeholders::_1));
		dispatcher.Dispatch<EventMouseScrolled>(std::bind(&EditorLayer::OnMouseScrolled, this, std::placeholders::_1));
		dispatcher.Dispatch<EventMouseButtonPressed>(std::bind(&EditorLayer::OnMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<EventViewportResize>(std::bind(&EditorLayer::OnViewportResize, this, std::placeholders::_1));
	}

	bool EditorLayer::OnKeyPressed(EventKeyPressed& e)
	{
		//CORE_TRACE(" EditorLayer::OnKeyPressed() called: {}", e);

		if (e.key == GLFW_KEY_ESCAPE)
			Application::GetInstance().Close();
		
		else if (e.key == GLFW_KEY_T)
		{
			auto& main_camera_entity = m_scene->FindEntityByName("Main Camera"s);
			auto& alt_camera_entity = m_scene->FindEntityByName("Alt Camera"s);

			ely::Entity controlled_camera_entity = m_scene->GetControlledCameraEntity();
			auto& tag = (std::string&)controlled_camera_entity.GetComponent<ely::TagComponent>();
			if (tag == "Main Camera"s)
				m_scene->SetControlledCameraEntity(alt_camera_entity);
			else
				m_scene->SetControlledCameraEntity(main_camera_entity);
		}

		return true;
	}

	bool EditorLayer::OnMouseMoved(EventMouseMoved& e)
	{
		//CORE_TRACE(" EditorLayer::OnMouseMoved() called: {}", e);
		m_scene->GetCameraController().OnMouseMoved(e);
		return true;
	}

	bool EditorLayer::OnMouseScrolled(EventMouseScrolled& e)
	{
		//CORE_TRACE(" EditorLayer::OnMouseScrolled() called: {}", e);
		m_scene->GetCameraController().OnMouseScrolled(e);
		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(EventMouseButtonPressed& e)
	{
		CORE_INFO(" EditorLayer::OnMouseButtonPressed() unmodified: {} {}", e.x, e.y);
		//Set the mouse position relative to the viewport
		//TODO - dubious?
		e.x = m_viewport_data.mouse_pos_viewport.x;
		e.y = m_viewport_data.mouse_pos_viewport.y;
		CORE_INFO(" EditorLayer::OnMouseButtonPressed() Updated: {} {}", e.x, e.y);
		m_scene->OnMouseButtonPressed(e);
		return true;
	}

	bool EditorLayer::OnViewportResize(EventViewportResize& e)
	{
		//TODO  - this is handled in the IMGui section below
		//CORE_TRACE(" EditorLayer::OnViewportResize() called: {}", e);
		//m_scene.OnViewportResize(e);
		return true;
	}

	void EditorLayer::OnImGuiRender()
	{
		//-----------------------------------------------------------------------------------------
		//setup docspace - see imgui_demo.cpp

		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		//ImVec2 p = ImGui::GetWindowViewport()->Pos;
		//CORE_WARN("DockSpace Viewport pos: {},{} ", p.x, p.y);
		
		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
				{ }
					

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{ }

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{ }

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{ }

				ImGui::Separator();

				if (ImGui::MenuItem("Exit"))
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		//TODO
		//Cherno reckons this is meant to come after all the windows (so that all panels are enclosed in the dockspace)
		// but not according to imgu_demo.cpp - still works if put here
		//ImGui::End(); 

		//-----------------------------------------------------------------------------------------

		ImGui::Begin("Render Stats");

		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("Width, Height %d %d : ", m_window.Width(), m_window.Height());
			ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window.BufferWidth(), m_window.BufferHeight());
			ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		if (ImGui::CollapsingHeader("Framebuffer - main camera"))
		{
			float tex_height = 400.0f;
			float tex_width = tex_height * m_viewport_data.size.x / m_viewport_data.size.y;
			uint64_t color_attachment_id = (uint64_t)m_framebuffer.GetColourAttachmentID(); //uint64_t to stop compiler warning
			ImTextureID tex_id = (void*)color_attachment_id;
			ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
		}

		if (ImGui::CollapsingHeader("Framebuffer - alt camera"))
		{
			float tex_height = 400.0f;
			float tex_width = tex_height * m_viewport_data.size.x / m_viewport_data.size.y;
			uint64_t color_attachment_id = (uint64_t)m_framebuffer_alt.GetColourAttachmentID();
			ImTextureID tex_id = (void*)color_attachment_id;
			ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
		}

		ImGui::End(); //End of render stats the panel

		//-----------------------------------------------------------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		ImVec2 im_viewport_size = ImGui::GetContentRegionAvail();
		OpenGLRenderer::SetViewport(0,0, (uint32_t)im_viewport_size.x, (uint32_t)im_viewport_size.y);
		
		if (*(reinterpret_cast<glm::vec2*>(&im_viewport_size)) != m_viewport_data.size)
		{
			//m_viewport_size = { im_viewport_size.x, im_viewport_size.y };
			m_framebuffer.Reset((uint32_t)(im_viewport_size.x), (uint32_t)(im_viewport_size.y));
			//update gl viewport to be same size as ImGui viewport
			//TODO  - check this!!!
			OpenGLRenderer::SetViewport(0, 0, (uint32_t)im_viewport_size.x, (uint32_t)im_viewport_size.y);
			ely::EventViewportResize e{ (uint32_t)(im_viewport_size.x), (uint32_t)(im_viewport_size.y) };
			m_scene->OnViewportResize(e);
		}

		//update viewport data
		//https://github.com/ocornut/imgui/issues/3404
		m_viewport_data.size = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
		m_viewport_data.window_pos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
		m_viewport_data.viewport_pos = { ImGui::GetWindowViewport()->Pos.x, ImGui::GetWindowViewport()->Pos.y };
		m_viewport_data.cursor_screen_pos = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y };
		m_viewport_data.mouse_pos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

		m_viewport_data.mouse_pos_viewport = { ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x,
			ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y };

		m_viewport_data.forcused = ImGui::IsWindowFocused();
		m_viewport_data.hovered = ImGui::IsWindowHovered();

		Application::GetInstance().GetImGuiLayer().BlockEvents(!m_viewport_data.forcused || !m_viewport_data.hovered);

		// Need to remove call to ImGui::Image() for this to be displayed
		/*ImGui::Text("Focused: %i", m_viewport_data.forcused);
		ImGui::Text("Hovered: %i", m_viewport_data.hovered);
		ImGui::Text("Viewport size: %.1f %.1f", m_viewport_data.size.x, m_viewport_data.size.y);
		ImGui::Text("Win pos: %.1f %.1f", m_viewport_data.window_pos.x, m_viewport_data.window_pos.y);
		ImGui::Text("Main Viewport pos: %.1f %.1f", m_viewport_data.viewport_pos.x, m_viewport_data.viewport_pos.y);
		ImGui::Text("Mouse pos: %.1f %.1f", m_viewport_data.mouse_pos.x, m_viewport_data.mouse_pos.y);
		ImGui::Text("Cursor screen pos: %.1f %.1f", m_viewport_data.cursor_screen_pos.x, m_viewport_data.cursor_screen_pos.y);
		ImGui::Text("Mouse pos in viewport: %.2f %.2f", m_viewport_data.mouse_pos_viewport.x, m_viewport_data.mouse_pos_viewport.y);*/

		//TODO - needs a bit of a rework! App is setting data for the engine!!??
		m_scene->SetViewportSize(m_viewport_data.size);
	
		uint64_t color_attachment_id = (uint64_t)m_framebuffer.GetColourAttachmentID(); //uint64_t to stop compiler warning
		ImTextureID tex_id = (void*)color_attachment_id;
		ImGui::Image(tex_id, ImVec2(im_viewport_size.x, im_viewport_size.y), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's

		ImGui::End(); //End of viewport panel 
		ImGui::PopStyleVar();

		ImGui::End();  //end of (dockspace demo) window

		//demo window
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

}
