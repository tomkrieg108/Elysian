#pragma once

#include <memory>

//platform detection / check
#ifdef _WIN32
#define PLATFORM_WINDOWS
#else
#error "Unsupported platform!"
#endif


#ifdef DEBUG
#ifdef PLATFORM_WINDOWS
#define DEBUGBREAK() __debugbreak()
#else
#error "Platform doesn't support DEBUGBREAK"
#endif //PLATFORM_WINDOWS
#define ENABLE_ASSERTS
#else
#define DEBUGBREAK()
#endif // DEBUG

#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#ifdef ENABLE_ASSERTS
#define ASSERT(x) if(!(x)) DEBUGBREAK()	// __debugbreak is compiler specific
#else
#define ASSERT(x)
#endif // ENABLE_ASSERTS

namespace ely
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}



