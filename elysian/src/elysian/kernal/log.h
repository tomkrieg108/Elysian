#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace ely
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#ifndef DIST
	// Core log across
	#define CORE_TRACE(...)    ::ely::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define CORE_INFO(...)     ::ely::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define CORE_WARN(...)     ::ely::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define CORE_ERROR(...)    ::ely::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define CORE_CRITICAL(...) ::ely::Log::GetCoreLogger()->critical(__VA_ARGS__)

	// Client log macros
	#define APP_TRACE(...)         ::ely::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define APP_INFO(...)          ::ely::Log::GetClientLogger()->info(__VA_ARGS__)
	#define APP_WARN(...)          ::ely::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define APP_ERROR(...)         ::ely::Log::GetClientLogger()->error(__VA_ARGS__)
	#define APP_CRITICAL(...)      ::ely::Log::GetClientLogger()->critical(__VA_ARGS__)

#else
	// Core log across
	#define CORE_TRACE(...)    
	#define CORE_INFO(...)     
	#define CORE_WARN(...)     
	#define CORE_ERROR(...)    
	#define CORE_CRITICAL(...) 

	// Client log macros
	#define APP_TRACE(...)         
	#define APP_INFO(...)          
	#define APP_WARN(...)         
	#define APP_ERROR(...)        
	#define APP_CRITICAL(...)     
#endif