#include "pch.h"
#include "elysian/kernal/application.h"
#include "elysian/imgui/im_gui_layer.h"
#include "elysian/kernal/input.h"
#include "elysian/camera/camera.h"
#include "elysian/camera/camera_controller.h"
#include "elysian/kernal/log.h"
#include "elysian/events/event_dispatcher.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace ely {

	void CameraController::OnUpdate(double delta_time)
	{
		//If either the middle of right mouse key is pressed => disable the cursor - indicates camera move/control mode
		bool alt_pressed = Input::IsKeyPressed(GLFW_KEY_LEFT_ALT) || Input::IsKeyPressed(GLFW_KEY_RIGHT_ALT);
		auto& window = Application::GetInstance().GetWindow();

		if (Input::IsMousebuttonPressed(GLFW_MOUSE_BUTTON_MIDDLE) || Input::IsMousebuttonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			window.SetCursorEnabled(false);	//allows for unlimited mouse movements
			if (Input::IsMousebuttonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			{
				if (alt_pressed)
					m_input_mode = InputMode::TranslateZ;
				else
					m_input_mode = InputMode::RotateLocal;
			}
			else //middle button
			{
				m_input_mode = InputMode::TranslateXY;
			}
		}
		else if (Input::IsMousebuttonPressed(GLFW_MOUSE_BUTTON_LEFT) && alt_pressed)
		{
			window.SetCursorEnabled(false);
			m_input_mode = InputMode::RotateWorld;
		}
		else
		{
			window.SetCursorEnabled(true); //cursor confined to window bounds
			m_input_mode = InputMode::Select;
		}

		if (ImGuiLayer::WantCaptureKeyboard())
			return;

		const float move_speed = 5.0f;
		const float move_amount = move_speed * (float)(delta_time);

		if (Input::IsKeyPressed(GLFW_KEY_W))
			MoveForward(-move_amount); //note the negative value needed to move forward - differs from the camera above

		if (Input::IsKeyPressed(GLFW_KEY_S))
			MoveForward(move_amount);//note the positive value needed to move backward - differs from the camera above

		if (Input::IsKeyPressed(GLFW_KEY_A))
			MoveRight(-move_amount);

		if (Input::IsKeyPressed(GLFW_KEY_D))
			MoveRight(move_amount);
	}

	void CameraController::MoveForward(float amount)
	{
		//TODO might be worth making camera_transform a member
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		camera_transform = glm::translate(camera_transform, glm::vec3(0, 0, amount));
	}

	void CameraController::MoveRight(float amount)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		camera_transform = glm::translate(camera_transform, glm::vec3(amount, 0, 0));
	}

	void CameraController::MoveVertically(float amount)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		glm::vec3 up = glm::vec3{ glm::inverse(camera_transform) * glm::vec4{0,1,0,0} }; //global up in camera space
		camera_transform = glm::translate(camera_transform, amount * up);
	}

	void CameraController::InvertPitch()
	{
		//TODO
	}

	//NOTE - not used
	void CameraController::UpdateTransform(float delta_yaw, float delta_pitch, glm::mat4& transform_)
	{
		//TODO - attempt to extract pitch and yaw from transform, then using the Update() code from previous projects
		// was trying to use this in RotateLocal()
		//Not really working just yet!

		auto& transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());

		//get current local front & right
		glm::vec3 front = glm::normalize(glm::vec3(transform[2][0], transform[2][1], transform[2][2])); //3rd col (local z / front axis)
		glm::vec3 right = glm::normalize(glm::vec3(transform[0][0], transform[0][1], transform[0][2])); //1st col (local x / right axis)

		glm::vec3 world_up = glm::vec3{ 0,1,0 };
		glm::vec3 world_right = glm::vec3{ 1,0,0 };

		float yaw = glm::orientedAngle(right, world_right, glm::cross(right, world_right)); //radians
		yaw = glm::degrees(yaw) - 90.0f; //degrees //initialy 0
		yaw += delta_yaw;

		float pitch = glm::orientedAngle(front, world_up, glm::cross(front, world_up)); //radians
		pitch = glm::degrees(pitch) - 90.0f; //degrees //initialy -90
		pitch += delta_pitch;

		/*if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;*/

		CORE_WARN("new pitch (z rot): {}", pitch);
		CORE_INFO("new yaw (x rot): {}", yaw);

		//update to new local front
		/*front.x = cos(glm::radians(camera.m_yaw)) * cos(glm::radians(camera.m_pitch));
		front.y = sin(glm::radians(camera.m_pitch));
		front.z = sin(glm::radians(camera.m_yaw)) * cos(glm::radians(camera.m_pitch));*/

		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		right = glm::normalize(glm::cross(front, world_up));
		glm::vec3 up = glm::normalize(glm::cross(right, front));

		//update to new local front, up, right
		transform[2][0] = front.x, transform[2][1] = front.y, transform[2][2] = front.z;			//3st col (local z/front axis)
		transform[1][0] = up.x, transform[1][1] = up.y, transform[1][2] = up.z;								//2st col (local y/up axis)
		transform[0][0] = right.x, transform[0][1] = right.y, transform[0][2] = right.z;			//1st col (local x/right axis)
	}

	void CameraController::Turn(float delta_yaw, float delta_pitch)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		glm::vec3 camera_front = camera.GetFront(camera_transform);
		glm::vec3 world_up = glm::vec3(0, 1, 0);

		float pitch_angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		pitch_angle = glm::degrees(pitch_angle);
		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp

		glm::mat4 rot_y = GetRotationMatY(-delta_yaw);
		camera_transform = rot_y * camera_transform;  //rotate about Y world axis

		if (pitch_angle > 175 && delta_pitch > 0)
			return;
		if (pitch_angle < 5 && delta_pitch < 0)
			return;

		glm::mat4 rot_x = GetRotationMatX(delta_pitch);
		camera_transform = camera_transform * rot_x;  //rotate about X local axis (lool up & down)
	}

	void CameraController::RotateLocal(float delta_yaw, float delta_pitch)
	{
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());

		glm::vec3 camera_front = camera.GetFront(camera_transform);
		glm::vec3 world_up = glm::vec3{ 0, 1, 0 };
		glm::vec4 world_up_4 = glm::vec4{ 0, 1, 0, 0 };

		glm::vec3 rot_axis = glm::vec3{ glm::inverse(camera_transform) * world_up_4 }; //global Y Axis in camera space
		camera_transform = glm::rotate(camera_transform, -delta_yaw, rot_axis);

		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp
		float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		angle = glm::degrees(angle);
		if (angle > 175 && delta_pitch > 0) return;
		if (angle < 5 && delta_pitch < 0) return;

		glm::mat4 rot_x = GetRotationMatX(delta_pitch);
		camera_transform = camera_transform * rot_x;  // X Axis in camera space
	}

	void CameraController::RotateWorld(float amount_x, float amount_y)
	{
		//only rotates allows for orbiting about world y axis
		auto& camera_transform = (glm::mat4&)(m_camera_entity.GetComponent<TransformComponent>());
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		glm::vec3 camera_front = camera.GetFront(camera_transform);
		glm::vec3 world_up = glm::vec3(0, 1, 0);

		float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		angle = glm::degrees(angle);
		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp

		glm::mat4 rot_y = GetRotationMatY(-amount_x);
		camera_transform = rot_y * camera_transform;  //orbit about Y global axis

		if (angle > 175 && amount_y > 0)
			return;
		if (angle < 5 && amount_y < 0)
			return;

		//glm::mat4 rot_x = GetRotationMatX(amount_y);
		//camera_transform = camera_transform * rot_x;  //rotate about X local axis
	}

	//NOTE: NOT USED
	void CameraController::RotateWorld2(float amount_x, float amount_y)
	{
		//Make grid a local object in the world and rotate it
		//All other objects need to be transformed relative to the grid transform
		//This doesn't work atm!

		auto& grid_transform = (glm::mat4&)(m_grid_entity.GetComponent<TransformComponent>());

		glm::vec3 world_up = glm::vec3{ 0, 1, 0 };
		glm::vec4 world_up_4 = glm::vec4{ 0, 1, 0, 0 };
		glm::vec3 world_right = glm::vec3{ 1, 0, 0 };
		glm::vec4 world_right_4 = glm::vec4{ 1, 0, 0, 0 };

		//glm::mat4 inv_grid_transform = glm::inverse(grid_transform);

		glm::vec3 rot_axis = glm::vec3{ glm::inverse(grid_transform) * world_up_4 }; //global Y Axis in camera space
		grid_transform = glm::rotate(grid_transform, -amount_x, rot_axis);

		//glm::mat4 rot_y = GetRotationMatY(-amount_x);
		//grid_transform = rot_y * grid_transform;  //rotate about gloabl Y world axis

		rot_axis = glm::vec3{ glm::inverse(grid_transform) * world_right_4 }; //global X Axis in grid space
		grid_transform = glm::rotate(grid_transform, -amount_y, rot_axis);
	}

	void CameraController::OnMouseMoved(EventMouseMoved& e)
	{
		// this if statement was triggering if the first mouse action was right button down (local camera turn) - weird!
		// preventing camera panning with right mouse held down initially
		//if (ImGuiLayer::WantCaptureMouse())
		//	return;
		switch (m_input_mode)
		{
		case InputMode::RotateWorld:
			RotateWorld(e.delta_x * 0.03f, e.delta_y * 0.03f); break;
			//RotateWorld2(e.delta_x * 0.03f, e.delta_y * 0.001f); break; 
		case 	InputMode::RotateLocal:
			RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f); break;
		case InputMode::TranslateXY:
			MoveRight(e.delta_x * 0.005f);
			MoveVertically(e.delta_y * 0.005f);
			break;
		case InputMode::TranslateZ:
			MoveForward((e.delta_x + e.delta_y) * 0.03f); break;
		};
	}

	void CameraController::OnMouseScrolled(EventMouseScrolled& e)
	{
		auto& camera = (PerspectiveCamera&)(m_camera_entity.GetComponent<PerspectiveCameraComponent>());
		camera.Zoom(e.y_offset);
	}

	//TODO - this thould be somewhere else - or just use glm!
	//angle in degrees
	glm::mat4 CameraController::GetRotationMatX(float angle) const
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
	glm::mat4 CameraController::GetRotationMatY(float angle) const
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
	glm::mat4 CameraController::GetRotationMatZ(float angle) const
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