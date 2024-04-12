#pragma once
#include "elysian/kernal/base.h"

namespace ely
{
	 namespace events_v1 {

		enum class EventType : uint32_t
		{
			None,
			WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
			ViewportResize,
			AppTick, AppUpdate, AppRender, //TODO - used ?
			KeyPressed, KeyReleased, KeyTyped,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
			Count
		};

		//NOTE: # prefex converts a macro parameter to a string literal without expanding the parameter defn:
		//https://learn.microsoft.com/en-us/cpp/preprocessor/stringizing-operator-hash?view=msvc-170

#define STATIC_EVENT_TYPE(type) static_cast<uint32_t>(EventType::type)  

		struct Event
		{
			virtual ~Event() = default;
			virtual EventType Type() { return EventType::None; }
			virtual const char* GetName() const { return ""; }
			virtual std::string ToString() const { return GetName(); }
			static uint32_t Count() { return STATIC_EVENT_TYPE(Count); }
			bool handled = false;
		};

		struct EventKeyPressed : public Event
		{
			EventKeyPressed() : key{ 0 } {}
			EventKeyPressed(int32_t key) : key{ key } {}
			EventType Type() override { return EventType::KeyPressed; }
			const char* GetName() const { return "Key Pressed"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(KeyPressed); }
			int32_t key;
		};

		struct EventKeyReleased : public Event
		{
			EventKeyReleased() : key{ 0 } {}
			EventKeyReleased(int32_t key) : key{ key } {}
			EventType Type() override { return EventType::KeyReleased; }
			const char* GetName() const { return "Key Released"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(KeyReleased); }
			int32_t key;
		};

		struct EventMouseMoved : public Event
		{
			EventMouseMoved() : x{ 0 }, y{ 0 }, delta_x{ 0 }, delta_y{ 0 } {}
			EventMouseMoved(float x, float y, float delta_x, float delta_y) :
				x{ x }, y{ y }, delta_x{ delta_x }, delta_y{ delta_y } {}
			EventType Type() override { return EventType::MouseMoved; }
			const char* GetName() const { return "Mouse Moved"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseMoved); }
			float x, y;
			float delta_x, delta_y;
		};

		struct EventMouseScrolled : public Event
		{
			EventMouseScrolled() : x_offset{ 0 }, y_offset{ 0 } {}
			EventMouseScrolled(float x_offset, float y_offset) :
				x_offset{ x_offset }, y_offset{ y_offset } {}
			EventType Type() override { return EventType::MouseScrolled; }
			const char* GetName() const { return "Mouse Scrolled"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseScrolled); }
			float x_offset, y_offset;
		};

		struct EventMouseButtonPressed : public Event
		{
			EventMouseButtonPressed() : x{ 0 }, y{ 0 }, btn{ 0 }, action{ 0 } {}
			EventMouseButtonPressed(float x, float y, int btn, int action) : x{ x }, y{ y }, btn{ btn }, action{ action } {}
			EventType Type() override { return EventType::MouseButtonPressed; }
			const char* GetName() const { return "Mouse Button Pressed"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseButtonPressed); }
			float x, y;
			int btn, action;
		};

		struct EventMouseButtonReleased : public Event
		{
			EventMouseButtonReleased() : x{ 0 }, y{ 0 }, btn{ 0 }, action{ 0 } {}
			EventMouseButtonReleased(float const x, float const y, int const btn, int const action) : x{ x }, y{ y }, btn{ btn }, action{ action } {}
			EventType Type() override { return EventType::MouseButtonReleased; }
			const char* GetName() const { return "Mouse Button Releasd"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseButtonReleased); }
			float x, y;
			int btn, action;
		};

		struct EventWidowResize : public Event
		{
			EventWidowResize() : buffer_width{ 0 }, buffer_height{ 0 } {}
			EventWidowResize(uint32_t const _buffer_width, uint32_t const _buffer_height) :
				buffer_width{ _buffer_width }, buffer_height{ _buffer_height } {}
			EventType Type() override { return EventType::WindowResize; }
			const char* GetName() const { return "Window Resized"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(WindowResize); }
			uint32_t buffer_width, buffer_height;
		};

		struct EventViewportResize : public Event
		{
			EventViewportResize() = default;
			EventViewportResize(uint32_t const width, uint32_t const height) :
				width{ width }, height{ height } {}
			EventType Type() override { return EventType::ViewportResize; }
			const char* GetName() const { return "Viewport Resized"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(ViewportResize); }
			uint32_t width = 0, height = 0;
		};

		struct EventWindowMoved : public Event
		{
			EventWindowMoved() = default;
			EventWindowMoved(uint32_t const xpos, uint32_t const ypos) :
				xpos{ xpos }, ypos{ ypos } {}
			EventType Type() override { return EventType::ViewportResize; }
			const char* GetName() const { return "Window Moved"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(ViewportResize); }
			uint32_t xpos = 0, ypos = 0;
		};

		struct EventWindowFocus : public Event
		{
			EventWindowFocus() = default;
			EventWindowFocus(uint32_t const focused) :
				focused{ focused } {}
			EventType Type() override { return EventType::ViewportResize; }
			const char* GetName() const { return "Window Focused"; }
			static uint32_t StaticType() { return STATIC_EVENT_TYPE(ViewportResize); }
			uint32_t focused = 0;
		};


		//Allows it to be used with spdlog
		inline std::ostream& operator<<(std::ostream& os, const Event& e)
		{
			return os << e.ToString();
		}

	}
}

namespace ely {

	inline namespace events_v2 {

		enum class EventType
		{
			None,
			WindowClose, WindowResize, WindowMove, WindowFocusChange, WindowHoverChange, WindowIconifyChange,
			ViewportResize,
			KeyPressed, KeyReleased, KeyTyped,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		};

		enum EventCategory
		{
			None = 0,
			EventCategoryApplication = BIT(0),
			EventCategoryInput = BIT(1),
			EventCategoryKeyboard = BIT(2),
			EventCategoryMouse = BIT(3),
			EventCategoryMouseButton = BIT(4)
		};

		//NOTE: # prefex converts a macro parameter to a string literal without expanding the parameter defn:
		//https://learn.microsoft.com/en-us/cpp/preprocessor/stringizing-operator-hash?view=msvc-170


		struct Event
		{
			virtual ~Event() = default;
			virtual EventType Type() const { return EventType::None; }
			virtual int GetCategoryFlags() const = 0;
			virtual const char* GetName() const { return ""; }
			virtual std::string ToString() const { return GetName(); }
			bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
			
			bool handled = false;
		};

		//------------------------------------------------------------------
		// Window events
		//------------------------------------------------------------------

		struct EventWidowClose : public Event
		{
			EventWidowClose() = default;
			EventType Type() const override { return EventType::WindowClose; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowClose; }
			const char* GetName() const { return "Window Close"; }
		};

		struct EventWidowResize : public Event
		{
			EventWidowResize() = delete;
			EventWidowResize(uint32_t buffer_width, uint32_t buffer_height) : buffer_width{ buffer_width }, buffer_height{ buffer_height } {}

			EventType Type() const override { return EventType::WindowResize; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowResize; }
			const char* GetName() const { return "Window Resized: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "( " + std::to_string(buffer_width) + "," + std::to_string(buffer_height) + ")";
				return s;
			}

			uint32_t buffer_width = 0, buffer_height = 0;
		};

		struct EventWindowFocusChange : public Event
		{
			EventWindowFocusChange() = delete;
			EventWindowFocusChange(int focused) : has_focus { focused } {}

			EventType Type() const override { return EventType::WindowFocusChange; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowFocusChange; }
			const char* GetName() const { return "Window Focus Change: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s +=  std::to_string(has_focus);
				return s;
			}

			int has_focus = 0;
		};

		struct EventWindowHoverChange : public Event
		{
			EventWindowHoverChange() = delete;
			EventWindowHoverChange(int hovered) : hovered{ hovered } {}

			EventType Type() const override { return EventType::WindowHoverChange; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowHoverChange; }
			const char* GetName() const { return "Window Hover Change: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += std::to_string(hovered);
				return s;
			}

			int hovered = 0;
		};

		struct EventWindowIconifyChange : public Event
		{
			EventWindowIconifyChange() = delete;
			EventWindowIconifyChange(int iconified) : is_iconified{ iconified } {}

			EventType Type() const override { return EventType::WindowIconifyChange; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowIconifyChange; }
			const char* GetName() const { return "Window Iconify Change: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += std::to_string(is_iconified);
				return s;
			}

			int is_iconified = 0;
		};

		
		struct EventWindowMove : public Event
		{
			EventWindowMove() = delete;
			EventWindowMove(uint32_t xpos, uint32_t ypos) : xpos{ xpos }, ypos{ ypos } {}

			EventType Type() const override { return EventType::WindowMove; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::WindowMove; }
			const char* GetName() const { return "Window Moved: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "( " + std::to_string(xpos) + "," + std::to_string(ypos) + ")";
				return s;
			}

			uint32_t xpos = 0, ypos = 0;
		};


		//------------------------------------------------------------------
		// Viewport events
		//------------------------------------------------------------------

		struct EventViewportResize : public Event
		{
			EventViewportResize() = delete;
			EventViewportResize(uint32_t width, uint32_t height) : width{ width }, height{ height } {}

			EventType Type() const override { return EventType::ViewportResize; }
			int GetCategoryFlags() const override { return EventCategoryApplication; }
			static EventType StaticType() { return EventType::ViewportResize; }
			const char* GetName() const { return "Viewport Resized: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "( " + std::to_string(height) + "," + std::to_string(width) + ")";
				return s;
			}

			uint32_t width = 0, height = 0;
		};

		//------------------------------------------------------------------
		// Key events
		//------------------------------------------------------------------

		struct EventKeyPressed : public Event
		{
			EventKeyPressed() = delete;
			EventKeyPressed(int32_t key, bool repeated = false) : key{ key }, repeated{ repeated } {}

			EventType Type() const override { return EventType::KeyPressed; }
			int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }
			static EventType StaticType() { return EventType::KeyPressed; }
			const char* GetName() const { return "Key Pressed: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "key: " + std::to_string(key) + "Repeat? " +  std::to_string(repeated);
				return s;
			}
			
			int32_t key = 0;
			bool repeated = false;
		};

		struct EventKeyReleased : public Event
		{
			EventKeyReleased() = delete;
			EventKeyReleased(int32_t key) : key{ key } {}

			EventType Type() const override { return EventType::KeyReleased; }
			int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }
			static EventType StaticType() { return EventType::KeyReleased; }
			const char* GetName() const { return "Key Released: "; }
			
			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "key: " + std::to_string(key);
				return s;
			}

			int32_t key = 0;
		};

		//TODO need to record the repeat count?
		struct EventKeyTyped : public Event
		{
			EventKeyTyped() = delete;
			EventKeyTyped(int32_t key) : key{ key } {}

			EventType Type() const override { return EventType::KeyReleased; }
			int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }
			static EventType StaticType() { return EventType::KeyReleased; }
			const char* GetName() const { return "Key Typed: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "key: " + std::to_string(key);
				return s;
			}

			int32_t key;
		};

		//------------------------------------------------------------------
		// Mouse events
		// TODO - support horizontal scroll?
		//------------------------------------------------------------------

		struct EventMouseButtonPressed : public Event
		{
			EventMouseButtonPressed() = delete;
			EventMouseButtonPressed(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}

			EventType Type() const override { return EventType::MouseButtonPressed; }
			int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }
			static EventType StaticType() { return EventType::MouseButtonPressed; }
			const char* GetName() const { return "Mouse Button Pressed: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "Btn: " + std::to_string(btn) + " Pos: " + "( " + std::to_string(x) + "," + std::to_string(y) + ")";
				return s;
			}

			float x = 0, y = 0;
			int btn = 0;
		};

		struct EventMouseButtonReleased : public Event
		{
			EventMouseButtonReleased() = delete;
			EventMouseButtonReleased(float x, float y, int btn) : x{ x }, y{ y }, btn{ btn } {}

			EventType Type() const override { return EventType::MouseButtonReleased; }
			int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryMouseButton | EventCategoryInput; }
			static EventType StaticType() { return EventType::MouseButtonReleased; }
			const char* GetName() const { return "Mouse Button Releasd: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "Btn: " + std::to_string(btn) + " Pos: " + "( " + std::to_string(x) + "," + std::to_string(y) + ")";
				return s;
			}

			float x = 0, y = 0;
			int btn = 0;
		};

		struct EventMouseMoved : public Event
		{
			EventMouseMoved() = delete;
			EventMouseMoved(float delta_x, float delta_y) : delta_x{ delta_x }, delta_y{ delta_y } {}

			EventType Type() const override { return EventType::MouseMoved; }
			int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
			static EventType StaticType() { return EventType::MouseMoved; }
			const char* GetName() const { return "Mouse Moved: "; }
			
			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += " Delta: " + std::to_string(delta_x) + "," + std::to_string(delta_y) + ")";
				return s;
			}
			float delta_x, delta_y;
		};

		struct EventMouseScrolled : public Event
		{
			EventMouseScrolled() : x_offset{ 0 }, y_offset{ 0 } {}
			EventMouseScrolled(float x_offset, float y_offset) : x_offset{ x_offset }, y_offset{ y_offset } {}

			EventType Type() const override { return EventType::MouseScrolled; }
			int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }
			static EventType StaticType() { return EventType::MouseScrolled; }
			const char* GetName() const { return "Mouse Scrolled: "; }

			std::string ToString() const override
			{
				std::string s = std::string{ GetName() };
				s += "Offset: " + std::to_string(x_offset) + "," + std::to_string(y_offset) + ")";
				return s;
			}
			
			float x_offset, y_offset;
		};

		
		//Allows it to be used with spdlog
		inline std::ostream& operator<<(std::ostream& os, const Event& e)
		{
			return os << e.ToString();
		}

	}

}