
#include "elysian/kernal/entry_point.h"
#include "sandbox.h"

namespace ely {

	Sandbox::Sandbox() :
		Application("Sandbox")
	{
		test_layer4 = new TestLayer4();
		this->PushLayer(test_layer4);

		//gamma_test_layer = new GammaTestLayer(ely::Application::GetInstance().GetWindow());
		//this->PushLayer(gamma_test_layer);
	}

	Sandbox::~Sandbox()
	{
		//this->PopLayer(test_layer4);
	}

	void Sandbox::OnUpdate(double delta_time)
	{
	}

	Application* CreateApplication()
	{
		return new Sandbox();
	}

}