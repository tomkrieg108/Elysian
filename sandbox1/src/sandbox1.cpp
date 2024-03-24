
#include "elysian/kernal/entry_point.h"
#include "elysian/kernal/application.h"
#include "test_layer4.h"
#include "gamma_correction_test.h"

class Sandbox : public ely::Application
{
public:
	Sandbox() :
	ely::Application("Sandbox")
	{
		test_layer4 = new TestLayer4();
		this->PushLayer(test_layer4);

		//gamma_test_layer = new GammaTestLayer(ely::Application::GetInstance().GetWindow());
		//this->PushLayer(gamma_test_layer);
	}

	~Sandbox()
	{
		this->PopLayer(test_layer4);
	}
	
	void OnUpdate(double delta_time) override
	{
	}

private:
	TestLayer4* test_layer4 = nullptr;
	GammaTestLayer* gamma_test_layer = nullptr;
};

ely::Application* ely::CreateApplication()
{
	return new Sandbox();
}

