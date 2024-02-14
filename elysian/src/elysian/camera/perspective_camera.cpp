#include "pch.h"
#include "elysian/camera/perspective_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{
	PerspectiveCamera::PerspectiveCamera(float aspect_ratio) : m_aspect_ratio{ aspect_ratio }
	{
		//m_model = glm::translate(m_model, glm::vec3(0.0f, 3.0f, 15.0f));
		SetPosition(glm::vec3(0.0f, 3.0f, 15.0f));
	}

	PerspectiveCamera::PerspectiveCamera(float aspect_ratio, glm::vec3 position) : m_aspect_ratio{ aspect_ratio }
	{
		//m_model = glm::translate(m_model, position);
		SetPosition(position);
	}

	
	/*
	TODO 
	only needs to be recalculated and uploaded to gpu if viewport size or fov (zoom level) changes.
	can cache this matrix and use a dirty flag
	*/
	glm::mat4 PerspectiveCamera::GetProjMatrix() const
	{
		return glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near, m_far);
	}

	glm::mat4 PerspectiveCamera::GetInverseProjMatrix() const
	{
		glm::mat4 proj_mat = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near, m_far);
		return glm::inverse(proj_mat);
	}
	 
	/*
	TODO
	only changes if camera moves - use cache and use dirty flag
	can also calculate faster by splitting the matrix into product of rotation component and position component.
	Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4) 
	*/
	glm::mat4 PerspectiveCamera::GetViewMatrix() const
	{
		return glm::inverse(m_model);
	}

	const glm::mat4& PerspectiveCamera::GetModelMatrix() const
	{
		return m_model;
	}

	//glm uses column major format => access is m[col][row] = or m[col] to get whole col
	void PerspectiveCamera::SetPosition(glm::vec3& position)
	{
		m_model[3][0] = position[0];
		m_model[3][1] = position[1];
		m_model[3][2] = position[2];
		m_model[3][3] = 1.0f;
	}

	glm::vec3 PerspectiveCamera::GetPosition() const
	{
		//4th col (world pos)
		return glm::vec3(m_model[3][0], m_model[3][1], m_model[3][2]);
	}

	//first 3 column vectors shoud already be normalised (length doesn't change under rotation)
	glm::vec3 PerspectiveCamera::GetFront() const
	{
		//3rd col (local z axis)
		return glm::normalize(glm::vec3(m_model[2][0], m_model[2][1], m_model[2][2]));
	}

	glm::vec3 PerspectiveCamera::GetRight() const
	{
		//1st col (local x axis)
		return glm::normalize(glm::vec3(m_model[0][0], m_model[0][1], m_model[0][2]));
	}

	glm::vec3 PerspectiveCamera::GetUp() const
	{
		//2st col (local y axis)
		return glm::normalize(glm::vec3(m_model[1][0], m_model[1][1], m_model[1][2]));
	}

	void PerspectiveCamera::SetAspectRatio(float width, float height)
	{
		if (height == 0.0f)
			m_aspect_ratio = 1.0f;
		else
			m_aspect_ratio = width / height;
	}
}