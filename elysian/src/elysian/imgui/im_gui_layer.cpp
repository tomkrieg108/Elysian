#include "pch.h"
#include "elysian/kernal/application.h"
#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>
#include "elysian/imgui/im_gui_layer.h"

namespace ely
{
	ImGuiLayer::ImGuiLayer() :
		Layer("ImGuiLayer")
	{
	}

	void ImGuiLayer::OnAttach()
	{
		GLFWwindow* glfw_window = Application::GetInstance().GetWindow().GetWindowHandle();

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		const char* glsl_version = "#version 430";
		ImGui_ImplOpenGL3_Init(glsl_version);

		ImGuiIO& io = ImGui::GetIO(); (void)io; //??!!
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows - crashes program!
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		float fontSize = 22.0f;
		//note that file path is relative to working dir, which is where the proj file (core) is by default - see proj properties->debugging
		io.Fonts->AddFontFromFileTTF("../../assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("../../assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);
		ImGui::StyleColorsDark();
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Sandbox"); //start window
	}

	void ImGuiLayer::End()
	{
		
		ImGui::End(); //End of the window

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetInstance();
		//not sure what this does - is in Hazel!
		io.DisplaySize = ImVec2((float)app.GetWindow().Width(), (float)app.GetWindow().Height());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/*if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}*/
	}

	bool ImGuiLayer::WantCaptureMouse()
	{
		// don't pass mouse and keyboard presses further if an ImGui widget is active
		auto& io = ImGui::GetIO();
		return (bool)(io.WantCaptureKeyboard);
	}

	bool ImGuiLayer::WantCaptureKeyboard()
	{
		// don't pass mouse and keyboard presses further if an ImGui widget is active
		auto& io = ImGui::GetIO();
		return (bool)(io.WantCaptureMouse);
	}

}