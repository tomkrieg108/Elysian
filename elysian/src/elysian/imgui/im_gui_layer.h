#pragma once

#include "elysian/kernal/layer.h"

namespace ely
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void Begin();
		void End();

		void SetDarkThemeColors();

		void BlockEvents(bool block) { m_block_events = block; }

		static bool WantCaptureMouse();
		static bool WantCaptureKeyboard();

	private:
		bool m_block_events = true;
	};

}