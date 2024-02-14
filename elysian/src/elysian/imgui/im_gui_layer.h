#pragma once

#include "elysian/kernal/layer.h"

namespace ely
{
	/*class Window;

	class ImGuiLayer
	{
	public:
		ImGuiLayer(Window const & window);
		~ImGuiLayer() = default;
		void Init();
		void Render();
		void Shutdown();
		static bool WantCaptureMouse();
		static bool WantCaptureKeyboard();
	private:
		void ImGuiUpdate();
		Window const & m_window;
	};*/

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		
		void Begin();
		void End();

		static bool WantCaptureMouse();
		static bool WantCaptureKeyboard();
	};

}