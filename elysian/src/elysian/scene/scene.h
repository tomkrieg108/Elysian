#pragma once
#include "entity.h"

namespace ely
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void Init();

		void Begin() {}
		void End() {}
		void OnUpdate(double delta_time);
		void Draw();

		const auto& GetEntities() const { return m_entities; }

	private:
		std::vector<Ref<Entity>> m_entities;
	};

}