#pragma once
#include "elysian/kernal/base.h"
#include "elysian/kernal/log.h"
#include "elysian/kernal/application.h"

//extern ely::Application* ely::CreateApplication(); //This isn't needed - declared in application.h

int main()
{
	ely::Log::Init();

	auto app = ely::CreateApplication();

	app->Run();

	delete app;
}
