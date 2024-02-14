#pragma once
#include "elysian/camera/ortho_camera.h"
#include "elysian/events/events.h"
#include <cmath>
#include <glm/glm.hpp>

namespace ely
{
	class OrthoCameraController
	{
	public:
		OrthoCameraController() = default;
		OrthoCameraController(glm::vec4 extent);
		OrthoCameraController(glm::vec4 extent, glm::vec3 position);
		~OrthoCameraController() = default;

		//TODO: for now the ortho camera will be setup so the it only looks down (in the -ve y dir)
		//later update so that it can change orientation as per perspective cam

		void MoveUp(float amount);
		void MoveRight(float amount);
		void MoveVertically(float amount);
		void Turn(float angle_ccw);
		void UpdateExtent(glm::vec4 extent);

		const OrthoCamera& GetCamera() const { return m_camera; }
		void OnUpdate(double delta_time);
		void OnMouseMoved(EventMouseMoved& e);
		void OnMouseScrolled(EventMouseScrolled& e);
		void OnWindowResize(EventWidowResize& e);
		void OnMouseButtonPressed(EventMouseButtonPressed& e);

	private:
		glm::mat4 GetRotationMatX(float angle) const;
		glm::mat4 GetRotationMatY(float angle) const;
		glm::mat4 GetRotationMatZ(float angle) const ;

	private:
		OrthoCamera m_camera; 
	};


}