#pragma once
#include "elysian/scene/scriptable_entity.h"

namespace ely {

	class NativeScriptRotateAndOrbit : public ScriptableEntity
	{
	public:
		virtual void OnUpdate(double timestep) override;
	};


}