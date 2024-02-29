
#include "elysian/kernal/entry_point.h"
#include "elysian/kernal/application.h"
#include "test_layer1.h"
#include "test_layer2.h"
#include "gamma_correction_test.h"

class Sandbox : public ely::Application
{
public:
	Sandbox() :
		ely::Application("Sandbox")
	{
		//test_layer1 = new TestLayer1(ely::Application::GetInstance().GetWindow());
		//this->PushLayer(test_layer1);

		test_layer2 = new TestLayer2(ely::Application::GetInstance().GetWindow());
		this->PushLayer(test_layer2);

		//gamma_test_layer = new GammaTestLayer(ely::Application::GetInstance().GetWindow());
		//this->PushLayer(gamma_test_layer);
	}

	~Sandbox()
	{
		this->PopLayer(test_layer1);
	}
	
	void OnUpdate(double delta_time) override
	{
	}

private:
	TestLayer1* test_layer1 = nullptr;
	TestLayer2* test_layer2 = nullptr;
	GammaTestLayer* gamma_test_layer = nullptr;
};

ely::Application* ely::CreateApplication()
{
	return new Sandbox();
}

