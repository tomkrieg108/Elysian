//see cherno video on logging for settinh up git sub modules in your project
#include "pch.h"
#include "elysian/kernal/log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ely
{

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("CORE_LOGGER");
		s_ClientLogger = spdlog::stdout_color_mt("CLIENT_LOGGER");

		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}



