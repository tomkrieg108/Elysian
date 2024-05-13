#pragma once

#include <glm/glm.hpp>

namespace ely 
{

	class Camera
	{
	public:

		enum class ProjectionType { Ortho, Perspective };

		struct PerspectiveParams
		{
			float z_near = 0.1f;
			float z_far = 1000.0f;
			float aspect_ratio = 1.0f;			// width / height
			float fov = 45.0f;							// Vert Field of view in degrees
			bool modified = true;
		};

		struct OrthoParams
		{
			float z_near = 0.1f;
			float z_far = 100.0f;
			float left = -20.0f, right = 20.0f, top = 20.0f, bottom = -20.0f;
			bool modified = true;
		};

	public:
		Camera();
		~Camera() = default;

		glm::mat4 GetProjMatrix() const;
		glm::mat4 GetInverseProjMatrix() const;
		glm::mat4 GetViewMatrix(const glm::mat4& transform_matrix) const;
		void SetAspectRatio(float width, float height);
		void Zoom(float amount);

		void SetProjectionType(ProjectionType projection_type) { m_projection_type = projection_type; }
		ProjectionType GetProjectionType() const {return m_projection_type;  }

		void SetParams(const PerspectiveParams& perspective_params);
		void SetParams(const OrthoParams& ortho_params);
		
		const auto& PerspectiveParameters() const { return m_perspective_params; }
		const auto& OrthoParameters() const { return m_ortho_params; }
		auto& PerspectiveParamseters() { return m_perspective_params; }
		auto& OrthoParameters() { return m_ortho_params; }
		
	private:
		ProjectionType m_projection_type = ProjectionType::Perspective;
		PerspectiveParams m_perspective_params;
		OrthoParams m_ortho_params;
	};

}


