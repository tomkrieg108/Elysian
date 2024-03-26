#pragma once
#include "elysian/kernal/base.h"

namespace ely
{
	enum class EventType : uint32_t
	{
		None,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		ViewportResize,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Count
	};

#define STATIC_EVENT_TYPE(type) static_cast<uint32_t>(EventType::type)  

	struct Event
	{
		virtual ~Event() = default;
		virtual EventType Type() { return EventType::None; }
		static uint32_t Count() { return STATIC_EVENT_TYPE(Count); }
		bool handled = false;
	};

	struct EventKeyPressed : public Event
	{
		EventKeyPressed() : key{ 0 } {}
		EventKeyPressed(int32_t key) : key{ key } {}
		EventType Type() override { return EventType::KeyPressed; }
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(KeyPressed);}
		int32_t key;
	};

	struct EventKeyReleased : public Event
	{
		EventKeyReleased() : key{ 0 } {}
		EventKeyReleased(int32_t key) : key{ key } {}
		EventType Type() override { return EventType::KeyReleased; }
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(KeyReleased); }
		int32_t key;
	};

	struct EventMouseMoved : public Event
	{
		EventMouseMoved() : x{ 0 }, y{ 0 }, delta_x{ 0 }, delta_y{ 0 } {}
		EventMouseMoved(float x, float y, float delta_x, float delta_y) :
			x{ x }, y{ y }, delta_x{ delta_x }, delta_y{ delta_y } {}
		EventType Type() override { return EventType::MouseMoved; }
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
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseScrolled); }
		float x_offset, y_offset;
	};

	struct EventMouseButtonPressed : public Event
	{
		EventMouseButtonPressed() : x{ 0 }, y{ 0 }, btn{ 0 }, action{ 0 } {}
		EventMouseButtonPressed(float x, float y, int btn, int action) : x{ x }, y{ y }, btn{ btn }, action{ action } {}
		EventType Type() override { return EventType::MouseButtonPressed; }
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(MouseButtonPressed); }
		float x, y;
		int btn, action;
	};

	struct EventMouseButtonReleased : public Event
	{
		EventMouseButtonReleased() : x{ 0 }, y{ 0 }, btn{ 0 }, action{ 0 } {}
		EventMouseButtonReleased(float const x, float const y, int const btn, int const action) : x{ x }, y{ y }, btn{ btn }, action{ action } {}
		EventType Type() override { return EventType::MouseButtonReleased; }
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
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(WindowResize); }
		uint32_t buffer_width, buffer_height;
	};

	struct EventViewportResize : public Event
	{
		EventViewportResize() = default;
		EventViewportResize(uint32_t const width, uint32_t const height) :
			width{ width }, height{ height } {}
		EventType Type() override { return EventType::ViewportResize; }
		static uint32_t StaticType() { return STATIC_EVENT_TYPE(ViewportResize); }
		uint32_t width = 0, height = 0;
	};

}