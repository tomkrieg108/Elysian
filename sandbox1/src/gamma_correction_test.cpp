#include "pch.h"

#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "gamma_correction_test.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

GammaTestLayer::GammaTestLayer(ely::Window& window) :
	m_window(window),
	m_camera_controller(glm::vec4(-25.0f, 25.0f, 25.0f, -25.0f), glm::vec3(0.0, 3.0, 0.0))
{
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // AOGL p20
	glEnable(GL_LINE_SMOOTH);

	m_window.SetClearColour(glm::vec4(0.6f, 0.6, 0.0f, 1.0f));

	//register event handlers
	ely::EventDispatcher::SetCallback(this, &GammaTestLayer::OnKeyPressed);
	ely::EventDispatcher::SetCallback(this, &GammaTestLayer::OnMouseMoved);
	ely::EventDispatcher::SetCallback(this, &GammaTestLayer::OnMouseScrolled);
	ely::EventDispatcher::SetCallback(this, &GammaTestLayer::OnWindowResize);
	ely::EventDispatcher::SetCallback(this, &GammaTestLayer::OnMouseButtonPressed);

	//buffer setup
	m_vbo_grid = ely::MeshPrimitive::GetGridVertexBuffer(20.0f, 1.0f);
	m_vbo_world_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 20.0f);
	m_vbo_gray_scale = ely::MeshPrimitive::GetGrayScaleStripBuffer(false);
	m_vbo_gray_scale_gc = ely::MeshPrimitive::GetGrayScaleStripBuffer(true); //gamma corrected

	m_vao_grid.AddVertexBuffer(*m_vbo_grid);
	m_vao_world_coords.AddVertexBuffer(*m_vbo_world_coords);
	m_vao_gray_scale.AddVertexBuffer(*m_vbo_gray_scale);
	m_vao_gray_scale_gc.AddVertexBuffer(*m_vbo_gray_scale_gc);

	ely::OpenGLTexture2D::Params tex_params;
	tex_params.enable_srgb = true;

	//framebuffer setup
	m_framebuffer = new ely::OpenGLFramebuffer(window.BufferWidth(), window.BufferHeight(), tex_params);

	//texture setup
	m_texture = new ely::OpenGLTexture2D("grass2.png");
	m_texture_srgb = new ely::OpenGLTexture2D("grass2.png", tex_params);

	//shader setup
	ely::ShaderBuilder shader_builder;
	m_gamma_test_shader = shader_builder.Add(ely::ShaderType::Vertex, "gamma.vs").Add(ely::ShaderType::Fragment, "gamma.fs").Build("Gamma Test Shader");
	m_gamma_test_shader->OutputInfo();
}

void GammaTestLayer::OnAttach()
{
}

void GammaTestLayer::OnDetach()
{
}

void GammaTestLayer::OnUpdate(double time_step)
{
	m_camera_controller.OnUpdate(time_step);

	//-----------------------------------------------------------------------------------
	//Render to screen (main camera)
	//-----------------------------------------------------------------------------------
	ely::OpenGLRenderer::SetLineWidth(1.0f);

	//grid
	m_gamma_test_shader->Bind();
	m_gamma_test_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));
	m_gamma_test_shader->SetUniformMat4f("u_view", m_camera_controller.GetCamera().GetViewMatrix());
	m_gamma_test_shader->SetUniformMat4f("u_proj", m_camera_controller.GetCamera().GetProjMatrix());

	m_vao_grid.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_grid->GetVertexCount());

	//global axes (uses same shader as grid)
	m_vao_world_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_world_coords->GetVertexCount());

	bool vs_gamma_enabled = false;

	//Uncorrected gray-scale script (res: appears linear on screen, but isn't)
	vs_gamma_enabled = false;
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale.Bind();
	glm::mat4 gray_scale_model_mat = glm::mat4(1.0f);
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected using fragment shader (res: appears skewed to the bright side on screen, 
	// but actual screen brightness is apparently linear 
	vs_gamma_enabled = true;
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected by enabling GL_FRAMEBUFFER_SRGB (res: same as above)
	vs_gamma_enabled = false;
	glEnable(GL_FRAMEBUFFER_SRGB);
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected in the mesh colours themselves (res: same as above)
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale_gc.Bind();
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale_gc->GetVertexCount());

	/////////////////////////////////////////////////////////////////////////////////

	m_framebuffer->Bind();
	m_window.Clear();
	

	//Uncorrected gray-scale script (res: appears linear on screen, but isn't)
	vs_gamma_enabled = false;
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale.Bind();
	gray_scale_model_mat = glm::mat4(1.0f);
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected using fragment shader (res: appears skewed to the bright side on screen, 
	// but actual screen brightness is apparently linear 
	vs_gamma_enabled = true;
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected by enabling GL_FRAMEBUFFER_SRGB (res: same as above)
	vs_gamma_enabled = false;
	glEnable(GL_FRAMEBUFFER_SRGB);
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale->GetVertexCount());

	//gamma corrected in the mesh colours themselves (res: same as above)
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale_gc.Bind();
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale_gc->GetVertexCount());

	m_framebuffer->Unbind();
}

void GammaTestLayer::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Width, Height %d %d : ", m_window.Width(), m_window.Height());
		ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window.BufferWidth(), m_window.BufferHeight());
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	if (ImGui::CollapsingHeader("Ortho Cam"))
	{
		const ely::OrthoCamera& camera = m_camera_controller.GetCamera();
		ImGui::Text("Otho Camera parameters");
		ImGui::SliderFloat3("Position ", &(camera.GetPosition()[0]), -20.0f, 20.0f);
		ImGui::SliderFloat3("Front ", &(camera.GetFront()[0]), -1.01f, 1.01f);
		ImGui::SliderFloat3("Up    ", &(camera.GetUp()[0]), -1.01f, 1.01f);
		ImGui::SliderFloat3("Right ", &(camera.GetRight()[0]), -1.01f, 1.01f);
	}
	if (ImGui::CollapsingHeader("Texture (SRGB disabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		ImTextureID tex_id = (void *)(m_texture->GetId());
		ImGui::Image(tex_id, ImVec2((int)tex_width, (int)tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Texture (SRGB enabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		ImTextureID tex_id = (void*)(m_texture_srgb->GetId());
		ImGui::Image(tex_id, ImVec2((int)tex_width, (int)tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Framebuffer (SRGB enabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		ImTextureID tex_id = (void*)(m_framebuffer->GetColourAttachmentID());
		ImGui::Image(tex_id, ImVec2((int)tex_width, (int)tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
}

void GammaTestLayer::OnKeyPressed(ely::EventKeyPressed& e)
{
	if (e.key == GLFW_KEY_ESCAPE)
		m_window.ShutDown();
	else if (e.key == GLFW_KEY_SPACE)
		m_window.ToggleCursorEnabled();
}

void GammaTestLayer::OnMouseMoved(ely::EventMouseMoved& e)
{
	if (!m_window.GetCursorEnabled())
		m_camera_controller.OnMouseMoved(e);
}

void GammaTestLayer::OnMouseScrolled(ely::EventMouseScrolled& e)
{
	m_camera_controller.OnMouseScrolled(e);
}

void GammaTestLayer::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
{
}

void GammaTestLayer::OnWindowResize(ely::EventWidowResize& e)
{
	m_camera_controller.OnWindowResize(e);
}
