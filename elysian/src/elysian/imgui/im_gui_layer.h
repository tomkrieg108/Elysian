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
		
		void Begin();
		void End();

		void SetDarkThemeColors();

		//uint32_t GetActiveWidgetID() const;

		static bool WantCaptureMouse();
		static bool WantCaptureKeyboard();
	};

}