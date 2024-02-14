//#pragma once

#include "elysian/events/events.h"

namespace ely
{

	template<typename E> using CallbackFn = std::function<void(E&)>;
#define CALLBACK_TYPEDECL(e) std::function<void(e&)> //not used
#define CALLBACK_TYPEDECL_2(t,e,fn_name) void (t::* fn_name)(e&) //not used

	class IEventCallback
	{
	public:
		virtual ~IEventCallback() = default;
		virtual void Dispatch() = 0;
		virtual void SetEvent(Event& event) = 0;
	};


	template<typename E>
	class EventCallback : public IEventCallback
	{
		//Use std::function
	public:
		EventCallback(CallbackFn<E>& callback_fn) : m_callback_fn{ callback_fn } {}
		void SetEvent(Event& event) override { m_event = static_cast<E&>(event); }
		void Dispatch() override { m_callback_fn(m_event); }

	private:
		E m_event;
		CallbackFn<E> m_callback_fn;
	};

	template<typename T, typename E>
	class EventCallback2 : public IEventCallback
	{
		//Use pointer to a class member function
	public:
		EventCallback2(T* instance, void (T::* callback_fn)(E&)) : m_instance(instance), m_callback_fn(callback_fn) {}
		void SetEvent(Event& event) override { m_event = static_cast<E&>(event); }
		void Dispatch() override { (m_instance->*m_callback_fn)(m_event); } //Think the '*' is optional
	private:
		E m_event;
		void (T::* m_callback_fn)(E&);
		T* m_instance;
	};


	class EventDispatcher
	{
	public:
		EventDispatcher(EventDispatcher& other) = delete;
		EventDispatcher& operator = (EventDispatcher& other) = delete;
		~EventDispatcher() = default;

		template<typename T, typename E>
		static void SetCallback(T* instance, void (T::* callback_fn)(E&))
		{
			IEventCallback* callback = EventDispatcher::MakeCallback(instance, callback_fn);
			s_instance.m_callback_list[E::StaticType()] = callback;
		}

		template<typename E>
		static void Dispatch(E& event)
		{
			ASSERT(s_instance.m_callback_list[E::StaticType()] != nullptr);

			s_instance.m_callback_list[E::StaticType()]->SetEvent(event);
			s_instance.m_callback_list[E::StaticType()]->Dispatch();
		}

		static EventDispatcher& Instance() { return s_instance; }

	private:

		template<typename T, typename E>
		static IEventCallback* MakeCallback(T* instance, void (T::* callback_fn)(E&))
		{
			//both of these work!

			//option 1
			CallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
			IEventCallback* opt1 = new EventCallback<E>(fn);

			//option 2
			IEventCallback* opt2 = new EventCallback2<T, E>(instance, callback_fn);

			return opt2;
		}

		EventDispatcher()
		{
			IEventCallback* default_callback = EventDispatcher::MakeCallback(this, &EventDispatcher::DefaultCallBack);
			for (uint32_t i = 0; i < m_callback_list.size(); i++)
			{
				if (m_callback_list[i] == nullptr)
				{
					m_callback_list[i] = default_callback;
				}
			}
		}
		void DefaultCallBack(Event& e)
		{
			//std::cout << "Default event callback\n";
		}

		std::array<IEventCallback*, STATIC_EVENT_TYPE(Count)> m_callback_list;
		static EventDispatcher s_instance;
	};

}