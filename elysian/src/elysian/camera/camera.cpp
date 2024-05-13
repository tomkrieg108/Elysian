#include "pch.h"
#include "elysian/kernal/base.h"
#include "elysian/kernal/application.h"
#include "elysian/camera/camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{

	Camera::Camera()
	{
		m_perspective_params.aspect_ratio = Application::GetInstance().GetWindow().AspectRatio();
	

	}
	
	glm::mat4 Camera::GetProjMatrix() const
	{
		if (m_projection_type == ProjectionType::Perspective)
		{
			return glm::perspective(glm::radians(m_perspective_params.fov), m_perspective_params.aspect_ratio,
				m_perspective_params.z_near, m_perspective_params.z_far);
		}
		else
		{
			return glm::ortho(m_ortho_params.left, m_ortho_params.right, m_ortho_params.bottom, m_ortho_params.top,
				m_ortho_params.z_near, m_ortho_params.z_far);
		}
	}

	glm::mat4 Camera::GetInverseProjMatrix() const
	{
		return glm::inverse(GetProjMatrix());
	}

	/*
	TODO
	Calculate faster by splitting the matrix into product of rotation component and position component.
	Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4)
	*/
	glm::mat4 Camera::GetViewMatrix(const glm::mat4& transform_matrix) const
	{
		return glm::inverse(transform_matrix);
	}

	void Camera::SetAspectRatio(float width, float height)
	{
		//TODO assert height not zero
		m_perspective_params.aspect_ratio = width / height;
		m_perspective_params.modified = true;
	}

	void Camera::Zoom(float amount)
	{
		if (m_projection_type == ProjectionType::Perspective)
		{
			float new_fov = m_perspective_params.fov + amount;
			new_fov > 75.0f ? m_perspective_params.fov = 75.0f : m_perspective_params.fov = new_fov;
			new_fov < 1.0f ? m_perspective_params.fov = 1.0f : m_perspective_params.fov = new_fov;
		}
	}

	void Camera::SetParams(const PerspectiveParams& perspective_params)
	{
		m_perspective_params = perspective_params;
		m_perspective_params.modified = true;
	}

	void Camera::SetParams(const OrthoParams& ortho_params)
	{
		m_ortho_params = ortho_params;
		m_ortho_params.modified = true;
	}

}
