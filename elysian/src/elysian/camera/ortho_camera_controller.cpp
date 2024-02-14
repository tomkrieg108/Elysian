#include "pch.h"
#include "elysian/imgui/im_gui_layer.h"
#include "elysian/kernal/input.h"
#include "elysian/camera/ortho_camera.h"
#include "elysian/camera/ortho_camera_controller.h"
#include "elysian/kernal/log.h"
#include "elysian/events/event_dispatcher.h"
#include "elysian/kernal/application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>

ely::OrthoCameraController::OrthoCameraController(glm::vec4 extent) :
  m_camera{extent}
{
	const Window& window = Application::GetInstance().GetWindow();
	m_camera.SetAspectRatio(window.Width(), window.Height());
}

ely::OrthoCameraController::OrthoCameraController(glm::vec4 extent, glm::vec3 position) :
  m_camera{extent, position}
{
	const Window& window = Application::GetInstance().GetWindow();
	m_camera.SetAspectRatio(window.Width(), window.Height());
}

void ely::OrthoCameraController::MoveUp(float amount)
{
	m_camera.m_model = glm::translate(m_camera.m_model, glm::vec3(0, amount, 0));
	//m_camera.m_model[3][1] += amount;
}

void ely::OrthoCameraController::MoveRight(float amount)
{
	m_camera.m_model = glm::translate(m_camera.m_model, glm::vec3(amount, 0, 0));
	//m_camera.m_model[3][0] += amount;
}

void ely::OrthoCameraController::MoveVertically(float amount)
{
}

void ely::OrthoCameraController::Turn(float angle_ccw)
{
	//rotate about y
	float angle_deg = glm::degrees(angle_ccw);
	glm::mat4 rot_z = GetRotationMatZ(angle_deg);
	m_camera.m_model = m_camera.m_model * rot_z;  //rotate about z local axis (which is pointing in the glabal y dir)
}

void ely::OrthoCameraController::UpdateExtent(glm::vec4 delta_extent)
{
	m_camera.m_extent += delta_extent;
}

void ely::OrthoCameraController::OnUpdate(double delta_time)
{
	if (ImGuiLayer::WantCaptureKeyboard())
		return;

	const float move_speed = 5.0f;
	const float move_amount = move_speed * (float)(delta_time);
	Input& input = Application::GetInstance().GetWindow().GetInput();

	if (input.IsKeyPressed(GLFW_KEY_W))
		MoveUp(-move_amount); 

	if (input.IsKeyPressed(GLFW_KEY_S))
		MoveUp(move_amount);

	if (input.IsKeyPressed(GLFW_KEY_A))
		MoveRight(-move_amount);

	if (input.IsKeyPressed(GLFW_KEY_D))
		MoveRight(move_amount);
}

void ely::OrthoCameraController::OnMouseMoved(EventMouseMoved& e)
{
	if (ImGuiLayer::WantCaptureMouse())
		return;

	const float sensitivity_x = 0.0015f;
	Turn(e.delta_x * sensitivity_x);
}

void ely::OrthoCameraController::OnMouseScrolled(EventMouseScrolled& e)
{
	glm::vec4 delta_extent = glm::vec4(e.y_offset, e.y_offset, e.y_offset, e.y_offset);
	UpdateExtent(delta_extent);
}

void ely::OrthoCameraController::OnWindowResize(EventWidowResize& e)
{
	m_camera.SetAspectRatio((float)e.buffer_width, (float)e.buffer_height);
}

void ely::OrthoCameraController::OnMouseButtonPressed(EventMouseButtonPressed& e)
{
}


//TODO:  exactly the same as in perspective_camera_cotroller.cpp!
glm::mat4 ely::OrthoCameraController::GetRotationMatX(float angle) const
{
	float angle_rads = glm::radians(angle);
	float c = std::cos(angle_rads);
	float s = std::sin(angle_rads);

	glm::mat4 m;
	m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
	m[0][1] = 0; m[1][1] = c; m[2][1] = -s; m[3][1] = 0;
	m[0][2] = 0; m[1][2] = s; m[2][2] = c; m[3][2] = 0;
	m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
	return m;
}

glm::mat4 ely::OrthoCameraController::GetRotationMatY(float angle) const
{
	float angle_rads = glm::radians(angle);
	float c = std::cos(angle_rads);
	float s = std::sin(angle_rads);

	glm::mat4 m;
	m[0][0] = c; m[1][0] = 0; m[2][0] = s; m[3][0] = 0;
	m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
	m[0][2] = -s; m[1][2] = 0; m[2][2] = c; m[3][2] = 0;
	m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
	return m;
}

glm::mat4 ely::OrthoCameraController::GetRotationMatZ(float angle) const
{
	float angle_rads = glm::radians(angle);
	float c = std::cos(angle_rads);
	float s = std::sin(angle_rads);

	glm::mat4 m;
	m[0][0] = c; m[1][0] = -s; m[2][0] = 0; m[3][0] = 0;
	m[0][1] = s; m[1][1] = c; m[2][1] = 0; m[3][1] = 0;
	m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
	m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
	return m;
}

