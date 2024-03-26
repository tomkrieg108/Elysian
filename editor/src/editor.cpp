
#include "elysian/kernal/entry_point.h"
#include "elysian/kernal/application.h"
#include "editor_layer.h"


namespace ely {

	class Editor : public Application
	{
	public:
		Editor() :
			Application("Editor")
		{
			editor_layer = new EditorLayer();
			this->PushLayer(editor_layer);
		}

		~Editor()
		{
			this->PopLayer(editor_layer);
		}

		void OnUpdate(double delta_time) override
		{
		}

	private:
		EditorLayer* editor_layer = nullptr;
	};

	Application* CreateApplication()
	{
		return new Editor();
	}

}

