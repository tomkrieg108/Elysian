#pragma once

#include "entity.h"

namespace ely {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}
	
	protected:
		virtual void OnCreate() {};
		virtual void OnDestroy() {};
		virtual void OnUpdate(double timestep) {};

	protected:
		Entity m_entity; 
		friend class Scene;
	};

}