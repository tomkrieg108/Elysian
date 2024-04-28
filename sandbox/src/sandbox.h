#pragma once
#include "elysian/kernal/application.h"
#include "test_layer4.h"
#include "gamma_correction_test.h"

//NOTE: This sandbox app might end up being the editor 

namespace ely {

	class Sandbox : public Application
	{
	public:
		Sandbox();
		virtual ~Sandbox();

		void OnUpdate(double delta_time) override;

	private:
		TestLayer4* test_layer4 = nullptr;
		GammaTestLayer* gamma_test_layer = nullptr;
	};
}