#include "pch.h"
#include "elysian/camera/ortho_camera.h"
#include "elysian/kernal/log.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{
	OrthoCamera::OrthoCamera(glm::vec4 extent) :
		m_extent{ extent }
	{
		//local z is global y direction
		m_model[2][0] = 0.0f;
		m_model[2][1] = 1.0f;
		m_model[2][2] = 0.0f;
		m_model[2][3] = 0.0f;

		//local x is in global z dir
		m_model[0][0] = 0.0f;
		m_model[0][1] = 0.0f;
		m_model[0][2] = 1.0f;
		m_model[0][3] = 0.0f;

		//local y is in global x dir
		m_model[1][0] = 1.0f;
		m_model[1][1] = 0.0f;
		m_model[1][2] = 0.0f;
		m_model[1][3] = 0.0f;

		SetPosition(glm::vec3(0,10,0.0));
	}

	OrthoCamera::OrthoCamera(glm::vec4 extent, glm::vec3 position) :
		m_extent{ extent }
	{
		//local z is global y direction
		m_model[2][0] = 0.0f;
		m_model[2][1] = 1.0f;
		m_model[2][2] = 0.0f;
		m_model[2][3] = 0.0f;

		//local x is in global z dir
		m_model[0][0] = 0.0f;
		m_model[0][1] = 0.0f;
		m_model[0][2] = 1.0f;
		m_model[0][3] = 0.0f;

		//local y is in global x dir
		m_model[1][0] = 1.0f;
		m_model[1][1] = 0.0f;
		m_model[1][2] = 0.0f;
		m_model[1][3] = 0.0f;

		SetPosition(position);
	}

	glm::mat4 OrthoCamera::GetProjMatrix() const
	{
		return glm::ortho(m_extent[0]*m_aspect_ratio, m_extent[1]* m_aspect_ratio, 
			m_extent[2], m_extent[3], m_near, m_far);
	}

	glm::mat4 OrthoCamera::GetInverseProjMatrix() const
	{
		glm::mat4 proj_mat = glm::ortho(m_extent[0]* m_aspect_ratio, m_extent[1]* m_aspect_ratio, 
			m_extent[2], m_extent[3], m_near, m_far);
		return glm::inverse(proj_mat);
	}

	//TODO: everything below is all the same as perspective camera!

	glm::mat4 OrthoCamera::GetViewMatrix() const
	{
		return glm::inverse(m_model);
	}

	const glm::mat4& OrthoCamera::GetModelMatrix() const
	{
		return m_model;
	}

	//glm uses column major format => access is m[col][row] = or m[col] to get whole col
	void OrthoCamera::SetPosition(glm::vec3& position)
	{
		m_model[3][0] = position[0];
		m_model[3][1] = position[1];
		m_model[3][2] = position[2];
		m_model[3][3] = 1.0f;
	}

	glm::vec3 OrthoCamera::GetPosition() const
	{
		//4th col (world pos)
		return glm::vec3(m_model[3][0], m_model[3][1], m_model[3][2]);
	}

	//first 3 column vectors shoud already be normalised (length doesn't change under rotation)
	glm::vec3 OrthoCamera::GetFront() const
	{
		//3rd col (local z axis)
		return glm::normalize(glm::vec3(m_model[2][0], m_model[2][1], m_model[2][2]));
	}

	glm::vec3 OrthoCamera::GetRight() const
	{
		//1st col (local x axis)
		return glm::normalize(glm::vec3(m_model[0][0], m_model[0][1], m_model[0][2]));
	}

	glm::vec3 OrthoCamera::GetUp() const
	{
		//2st col (local y axis)
		return glm::normalize(glm::vec3(m_model[1][0], m_model[1][1], m_model[1][2]));
	}

	void OrthoCamera::SetAspectRatio(float aspect_ratio)
	{
		//TODO assert aspect ratio in valid range
		m_aspect_ratio = aspect_ratio;
	}
}