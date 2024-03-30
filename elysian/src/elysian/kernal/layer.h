#pragma once

#include "elysian/events/events.h"
//from Hazel

namespace ely
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(double delta_time) {}
		virtual void OnEvent(Event& event) {};
		virtual void OnEvent(events_v2::Event& event) {};
		virtual void OnImGuiRender() {}
		const std::string& GetName() const { return m_debug_name; }
	protected:
		std::string m_debug_name;
	};

	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend() { return m_Layers.rend(); }

		auto begin() const { return m_Layers.begin(); }
		auto end()	const { return m_Layers.end(); }
		auto rbegin() const { return m_Layers.rbegin(); }
		auto rend() const { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}
