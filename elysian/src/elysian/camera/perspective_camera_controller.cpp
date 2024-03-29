#include "pch.h"
#include "elysian/kernal/application.h"
#include "elysian/imgui/im_gui_layer.h"
#include "elysian/kernal/input.h"
#include "elysian/camera/perspective_camera.h"
#include "elysian/camera/perspective_camera_controller.h"
#include "elysian/kernal/log.h"
#include "elysian/events/event_dispatcher.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace ely {

	void PerspectiveCameraController::OnUpdate(double delta_time)
	{
		//if (ImGuiLayer::WantCaptureKeyboard())
		//	return;

		const float move_speed = 5.0f;
		const float move_amount = move_speed * (float)(delta_time);
		Input& input = Application::GetInstance().GetWindow().GetInput();

		if (input.IsKeyPressed(GLFW_KEY_W))
			MoveForward(-move_amount); //note the negative value needed to move forward - differs from the camera above

		if (input.IsKeyPressed(GLFW_KEY_S))
			MoveForward(move_amount);//note the positive value needed to move backward - differs from the camera above

		if (input.IsKeyPressed(GLFW_KEY_A))
			MoveRight(-move_amount);

		if (input.IsKeyPressed(GLFW_KEY_D))
			MoveRight(move_amount);
	}


	void PerspectiveCameraController::MoveForward(float amount)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		camera_transform = glm::translate(camera_transform, glm::vec3(0, 0, amount));
	}

	void PerspectiveCameraController::MoveRight(float amount)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		camera_transform = glm::translate(camera_transform, glm::vec3(amount, 0, 0));
	}

	void PerspectiveCameraController::Turn(float delta_yaw, float delta_pitch)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		glm::vec3 camera_front = camera.GetFront(camera_transform);
		glm::vec3 world_up = glm::vec3(0, 1, 0);

		float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		angle = glm::degrees(angle);
		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp

		glm::mat4 rot_y = GetRotationMatY(-delta_yaw);
		camera_transform = rot_y * camera_transform;  //rotate about Y world axis

		if (angle > 175 && delta_pitch > 0)
			return;
		if (angle < 5 && delta_pitch < 0)
			return;

		glm::mat4 rot_x = GetRotationMatX(delta_pitch);
		camera_transform = camera_transform * rot_x;  //rotate about X local axis
	}

	void PerspectiveCameraController::Zoom(float amount)
	{
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());

		auto fov = camera.GetFov();
		camera.SetFov(fov + amount);
		if (camera.GetFov() > 75.0f)
			camera.SetFov(75.0f);
		if (camera.GetFov() < 10.0f)
			camera.SetFov(10.0f);
	}

	void PerspectiveCameraController::MoveVertically(float amount)
	{
		//needs to be done in world coords - not local camera coords
	}

	void PerspectiveCameraController::InvertPitch()
	{
	}

	void PerspectiveCameraController::OnMouseMoved(EventMouseMoved& e)
	{
		//if (ImGuiLayer::WantCaptureMouse())
		//	return;

		const float sensitivity_x = 0.03f;
		const float sensitivity_y = 0.05f;
		Turn(e.delta_x * sensitivity_x, e.delta_y * sensitivity_y);
	}

	void PerspectiveCameraController::OnMouseScrolled(EventMouseScrolled& e)
	{
		Zoom(e.y_offset);
	}

	void PerspectiveCameraController::OnMouseButtonPressed(EventMouseButtonPressed& e)
	{
	}

	void PerspectiveCameraController::OnWindowResize(EventWidowResize& e)
	{
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		camera.SetAspectRatio((float)e.buffer_width, (float)e.buffer_height);
	}

	void PerspectiveCameraController::OnViewportResize(EventViewportResize& e)
	{
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		camera.SetAspectRatio((float)e.width, (float)e.height);
	}

	//angle in degrees
	glm::mat4 PerspectiveCameraController::GetRotationMatX(float angle) const
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

	//angle in degrees
	glm::mat4 PerspectiveCameraController::GetRotationMatY(float angle) const
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

	//angle in degrees
	glm::mat4 PerspectiveCameraController::GetRotationMatZ(float angle) const
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
}