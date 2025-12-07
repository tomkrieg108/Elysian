#include "pch.h" //TODO - not using pch?

#include "elysian/kernal/application.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/renderer/opengl_shader.h"
#include "elysian/model/mesh_primitives.h"
#include "elysian/kernal/log.h"
#include "elysian/renderer/opengl_renderer.h"
#include "elysian/camera/camera.h"
#include "elysian/scene/entity.h"
#include "elysian/scene/component.h"
#include "gamma_correction_test.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui_docking/imgui.h>

GammaTestLayer::GammaTestLayer(ely::Window& window) :
	m_window(window),
	m_scene{ ely::CreateRef<ely::Scene>() }
{
	glLineWidth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // AOGL p20
	glEnable(GL_LINE_SMOOTH);

	m_window.SetClearColour(glm::vec4(0.6f, 0.6, 0.0f, 1.0f));

	m_camera_entity = m_scene->CreateCameraEntity(glm::vec3(4.0, 1.0, 7.0), "Camera"s);
	m_scene->SetRenderable(m_camera_entity, false);
	m_scene->SetControlledCameraEntity(m_camera_entity);

	//buffer setup
	m_vbo_grid = ely::MeshPrimitive::GetGridVertexBuffer(20.0f, 1.0f);
	m_vbo_world_coords = ely::MeshPrimitive::GetCoordSystemVertexBuffer(glm::mat4(1), 20.0f);
	m_vbo_gray_scale = ely::MeshPrimitive::GetGrayScaleStripBuffer(false);
	m_vbo_gray_scale_gc = ely::MeshPrimitive::GetGrayScaleStripBuffer(true); //gamma corrected

	m_vao_grid.AddVertexBuffer(m_vbo_grid);
	m_vao_world_coords.AddVertexBuffer(m_vbo_world_coords);
	m_vao_gray_scale.AddVertexBuffer(m_vbo_gray_scale);
	m_vao_gray_scale_gc.AddVertexBuffer(m_vbo_gray_scale_gc);

	ely::OpenGLTexture2D::Params tex_params;
	tex_params.enable_srgb = true;

	//framebuffer setup
	m_framebuffer = new ely::OpenGLFramebuffer({ window.BufferWidth(), window.BufferHeight(), tex_params });

	//texture setup
	m_texture = new ely::OpenGLTexture2D("grass2.png");
	m_texture_srgb = new ely::OpenGLTexture2D("grass2.png", tex_params);

	//shader setup
	m_gamma_test_shader = ely::ShaderRepo::Get("gamma");
}

void GammaTestLayer::OnAttach()
{
}

void GammaTestLayer::OnDetach()
{
}

void GammaTestLayer::OnEvent(ely::Event& e)
{
	ely::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<ely::EventKeyPressed>(std::bind(&GammaTestLayer::OnKeyPressed, this, std::placeholders::_1));
	dispatcher.Dispatch<ely::EventMouseMoved>(std::bind(&GammaTestLayer::OnMouseMoved, this, std::placeholders::_1));
	dispatcher.Dispatch<ely::EventMouseScrolled>(std::bind(&GammaTestLayer::OnMouseScrolled, this, std::placeholders::_1));
	dispatcher.Dispatch<ely::EventMouseButtonPressed>(std::bind(&GammaTestLayer::OnMouseButtonPressed, this, std::placeholders::_1));
	dispatcher.Dispatch<ely::EventWidowResize>(std::bind(&GammaTestLayer::OnWindowResize, this, std::placeholders::_1));
}


void GammaTestLayer::OnUpdate(double time_step)
{
	m_scene->GetCameraController().OnUpdate(time_step);
	
	//-----------------------------------------------------------------------------------
	//Render to screen (main camera)
	//-----------------------------------------------------------------------------------
	ely::OpenGLRenderer::SetLineWidth(1.0f);

	//grid

	auto& camera = (ely::Camera&)(m_camera_entity.GetComponent<ely::CameraComponent>());
	auto& camera_transform = (glm::mat4)(m_camera_entity.GetComponent<ely::TransformComponent>());
	//camera.SetProjectionType(ely::Camera::ProjectionType::Ortho);

	m_gamma_test_shader->Bind();
	m_gamma_test_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));
	m_gamma_test_shader->SetUniformMat4f("u_view", camera.GetViewMatrix(camera_transform));
	m_gamma_test_shader->SetUniformMat4f("u_proj", camera.GetProjMatrix());

	m_vao_grid.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_grid->GetVertexCount());

	//global axes (uses same shader as grid)
	m_vao_world_coords.Bind();
	//glDrawArrays(GL_LINES, 0, m_vbo_world_coords->GetVertexCount());

	bool vs_gamma_enabled = false;

	//Uncorrected gray-scale script (res: appears linear on screen, but isn't)
	vs_gamma_enabled = false;
	//glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale.Bind();
	glm::mat4 gray_scale_model_mat = glm::mat4(1.0f);
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected using fragment shader (res: appears skewed to the bright side on screen, 
	// but actual screen brightness is apparently linear 
	vs_gamma_enabled = true;
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected by enabling GL_FRAMEBUFFER_SRGB (res: same as above)
	vs_gamma_enabled = false;
	glEnable(GL_FRAMEBUFFER_SRGB);
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected in the mesh colours themselves (res: same as above)
	//glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale_gc.Bind();
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale_gc.GetVertexCount());

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
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected using fragment shader (res: appears skewed to the bright side on screen, 
	// but actual screen brightness is apparently linear 
	vs_gamma_enabled = true;
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected by enabling GL_FRAMEBUFFER_SRGB (res: same as above)
	vs_gamma_enabled = false;
	glEnable(GL_FRAMEBUFFER_SRGB);
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	m_gamma_test_shader->SetUniform1i("u_gamma", vs_gamma_enabled);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale.GetVertexCount());

	//gamma corrected in the mesh colours themselves (res: same as above)
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_vao_gray_scale_gc.Bind();
	gray_scale_model_mat = glm::translate(gray_scale_model_mat, glm::vec3(0, 0, 2.0f));
	m_gamma_test_shader->SetUniformMat4f("u_model", gray_scale_model_mat);
	glDrawArrays(GL_TRIANGLES, 0, m_vbo_gray_scale_gc.GetVertexCount());

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
		auto& camera = (ely::Camera&)(m_camera_entity.GetComponent<ely::CameraComponent>());
		auto& camera_transform = (glm::mat4)(m_camera_entity.GetComponent<ely::TransformComponent>());
		
		ImGui::Text("Otho Camera parameters");
		ImGui::SliderFloat3("Position ", &(camera_transform[3][0]), -20.0f, 20.0f);
		ImGui::SliderFloat3("Front ", &(camera_transform[2][0]), -20.0f, 20.0f);
		ImGui::SliderFloat3("Up ", &(camera_transform[1][0]), -20.0f, 20.0f);
		ImGui::SliderFloat3("Right ", &(camera_transform[0][0]), -20.0f, 20.0f);
	}
	if (ImGui::CollapsingHeader("Texture (SRGB disabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t tex_id = (uint64_t)(m_texture_srgb->GetId());
		ImGui::Image((void*)tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Texture (SRGB enabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t tex_id = (uint64_t)(m_texture_srgb->GetId());
		ImGui::Image((void *)tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
	if (ImGui::CollapsingHeader("Framebuffer (SRGB enabled)"))
	{
		float tex_height = 400.0f;
		float tex_width = tex_height * m_window.AspectRatio();
		uint64_t tex_id = (uint64_t)(m_texture_srgb->GetId());
		ImGui::Image((void *)tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); //need to flip uv's
	}
}


bool GammaTestLayer::OnKeyPressed(ely::EventKeyPressed& e)
{
	if (e.key == GLFW_KEY_ESCAPE)
		ely::Application::GetInstance().Close();
		
	return true;
}

bool GammaTestLayer::OnMouseMoved(ely::EventMouseMoved& e)
{
	m_scene->GetCameraController().OnMouseMoved(e);
	return true;
}

bool GammaTestLayer::OnMouseScrolled(ely::EventMouseScrolled& e)
{
	m_scene->GetCameraController().OnMouseScrolled(e);
	return true;
}

bool GammaTestLayer::OnMouseButtonPressed(ely::EventMouseButtonPressed& e)
{
	return true;
}

bool GammaTestLayer::OnWindowResize(ely::EventWidowResize& e)
{
	m_scene->OnWindowResize(e);
	return true;
}
