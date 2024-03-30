#pragma once

#include "elysian/kernal/log.h"
#include "elysian/events/events.h"

namespace ely
{
	//TODO = can set this up to be like an observer rather than event system
	//also setup so that can call free functions

	 inline namespace events_v1 {

		template<typename E> using EventCallbackFn = std::function<void(E&)>;

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
			EventCallback(EventCallbackFn<E>& callback_fn) : m_callback_fn{ callback_fn } {}
			void SetEvent(Event& event) override { m_event = static_cast<E&>(event); }
			void Dispatch() override { m_callback_fn(m_event); }

		private:
			E m_event;
			EventCallbackFn<E> m_callback_fn;
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

			template<typename T, typename E>
			static void SetCallback(T* instance, void (T::* callback_fn)(E&))
			{
				IEventCallback* callback = EventDispatcher::MakeCallback(instance, callback_fn);
				s_instance.m_callback_list[(uint32_t)E::StaticType()] = callback;
			}

			template<typename E>
			static void Dispatch(E& event)
			{
				ASSERT(s_instance.m_callback_list[E::StaticType()] != nullptr);

				s_instance.m_callback_list[(uint32_t)E::StaticType()]->SetEvent(event);
				s_instance.m_callback_list[(uint32_t)E::StaticType()]->Dispatch();
			}

			static EventDispatcher& Instance() { return s_instance; } //TODO - never used

		private:

			template<typename T, typename E>
			static IEventCallback* MakeCallback(T* instance, void (T::* callback_fn)(E&))
			{
				//both of these work!

				//option 1
				//NOTE can also ue lambdas in place of bind - see vid 186 of James raynards c++ course
				EventCallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
				std::function<void(E&)> fn2 = std::bind(callback_fn, instance, std::placeholders::_1);

				IEventCallback* opt1 = new EventCallback<E>(fn);

				//option 2
				IEventCallback* opt2 = new EventCallback2<T, E>(instance, callback_fn);

				return opt1;
			}


			void DefaultCallBack(Event& e)
			{
				CORE_TRACE("Default callback called");
			}

			std::array<IEventCallback*, STATIC_EVENT_TYPE(Count)> m_callback_list;
			static EventDispatcher s_instance;
		};

	}
}

namespace ely {
	/*
	Chernos version
	Event category not included for now
	*/
	 namespace events_v2 {

		using EventCallbackFn = std::function<void(Event&)>;

		class EventDispatcher
		{
		public:
			EventDispatcher(Event& event)
				: m_event(event)
			{
			}

			// F will be deduced by the compiler
			template<typename E, typename F>
			bool Dispatch(const F& func)
			{
				if (m_event.Type() == E::StaticType())
				{
					m_event.handled |= func(static_cast<E&>(m_event));
					//func(static_cast<E&>(m_event));
					return true;
				}
				return false;
			}

			//almost Exactly the same as Application::MakeCallback
			template<typename T, typename E>
			static EventCallbackFn MakeCallback(bool (T::* callback_fn)(E&), T* instance)
			{
				EventCallbackFn fn = std::bind(callback_fn, instance, std::placeholders::_1);
				return fn;
			}
		
		private:
			Event& m_event;
		};

	}

}

namespace ely {

#if 0
	namespace events_v3 {

		template<typename E> using EventCallbackFn = std::function<void(E&)>; //used in window.h


		class EventCallbackBase
		{
		public:
			virtual ~EventCallbackBase() = default;
			virtual void Dispatch() = 0;
			virtual void SetEvent(Event& event) = 0;

			template<typename E> E& GetEvent();
			template<typename E> EventCallbackFn<E>& GetCallbackFn();
		};

		template<typename E>
		class EventCallback : public EventCallbackBase
		{
		public:
			EventCallback(EventCallbackFn<E>& callback_fn) : m_callback_fn{ callback_fn } {}
			void SetEvent(E& event) { m_event = event; }
			void Dispatch() { m_callback_fn(m_event); }

			E& GetEvent() { return m_event; }
			EventCallbackFn<E> GetCallbackFn() { return m_callback_fn; }

		private:
			E m_event;
			EventCallbackFn<E> m_callback_fn;
		};

		template<typename E> E& EventCallbackBase::GetEvent() { return dynamic_cast<EventCallback<E>&>(this)->GetEvent(); }
		template<typename E> EventCallbackFn<E>& EventCallbackBase::GetCallbackFn() { return dynamic_cast<EventCallback<E>&>(this)->GetCallbackFn(); }

		//----------

		class EventDispatcher
		{
		public:
			EventDispatcher(EventDispatcher& other) = delete;
			EventDispatcher& operator = (EventDispatcher& other) = delete;
			~EventDispatcher() = default;

			template<typename T, typename E>
			static EventCallback<E>* MakeCallback(void (T::* callback_fn)(E&), T* instance)
			{
				EventCallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
				return new EventCallback<E>(fn);
			}
		private:

			//template<typename E, typename T>
			//static EventCallback<E>* MakeCallback2(T* instance, void (T::* callback_fn)(E&))
			//{
			//	//NOTE can also ue lambdas in place of bind - see vid 186 of James raynards c++ course
			//	EventCallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
			//	IEventCallback* opt1 = new EventCallback<E>(fn);
			//	return opt1;
			//}

			//template<typename T, typename E>
			//static IEventCallback* MakeCallback(T* instance, void (T::* callback_fn)(E&))
			//{
			//	//option 1
			//	//NOTE can also ue lambdas in place of bind - see vid 186 of James raynards c++ course
			//	EventCallbackFn<E> fn = std::bind(callback_fn, instance, std::placeholders::_1);
			//	IEventCallback* opt1 = new EventCallback<E>(fn);
			//	return opt1;
			//}
		};
	}

#endif
}