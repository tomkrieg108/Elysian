#include "pch.h"
#include "elysian/kernal/application.h"
#include "elysian/camera/perspective_camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ely
{
	PerspectiveCamera::PerspectiveCamera()
	{
		m_aspect_ratio = Application::GetInstance().GetWindow().AspectRatio();
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
	glm::mat4 PerspectiveCamera::GetViewMatrix(const glm::mat4& transform_matrix) const
	{
		return glm::inverse(transform_matrix);
	}

	//glm uses column major format => access is m[col][row] = or m[col] to get whole col
	void PerspectiveCamera::SetPosition(glm::vec3& position, glm::mat4& transform_matrix)
	{
		transform_matrix[3][0] = position[0];
		transform_matrix[3][1] = position[1];
		transform_matrix[3][2] = position[2];
		transform_matrix[3][3] = 1.0f;
	}

	glm::vec3 PerspectiveCamera::GetPosition(const glm::mat4& transform_matrix) const
	{
		//4th col (world pos)
		return glm::vec3(transform_matrix[3][0], transform_matrix[3][1], transform_matrix[3][2]);
	}

	//first 3 column vectors shoud already be normalised (length doesn't change under rotation)
	glm::vec3 PerspectiveCamera::GetFront(const glm::mat4& transform_matrix) const
	{
		//3rd col (local z axis)
		return glm::normalize(glm::vec3(transform_matrix[2][0], transform_matrix[2][1], transform_matrix[2][2]));
	}

	glm::vec3 PerspectiveCamera::GetRight(const glm::mat4& transform_matrix) const
	{
		//1st col (local x axis)
		return glm::normalize(glm::vec3(transform_matrix[0][0], transform_matrix[0][1], transform_matrix[0][2]));
	}

	glm::vec3 PerspectiveCamera::GetUp(const glm::mat4& transform_matrix) const
	{
		//2st col (local y axis)
		return glm::normalize(glm::vec3(transform_matrix[1][0], transform_matrix[1][1], transform_matrix[1][2]));
	}

	void PerspectiveCamera::SetAspectRatio(float width, float height)
	{
		if (height == 0.0f)
			m_aspect_ratio = 1.0f;
		else
			m_aspect_ratio = width / height;
	}
}