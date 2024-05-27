#include "pch.h"
#include "rotate_and_orbit.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ely {

	void NativeScriptRotateAndOrbit::OnUpdate(double timestep)
	{
		glm::mat4& transform = (glm::mat4&)m_entity.GetComponent<TransformComponent>();
		float angle = (float)timestep; // 1/60 radians 
		glm::vec3 rot_axis{ 1.0f,1.0f,1.0f };
		transform = glm::rotate(transform, angle*0.7f, rot_axis);
		//orbit box about world y axis
		glm::mat4 orbit_transform = glm::mat4(1.0f);
		orbit_transform = glm::rotate(orbit_transform, angle*0.2f, glm::vec3(0, 1, 0));
		transform = orbit_transform * transform;
	}

}