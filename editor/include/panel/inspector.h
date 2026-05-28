#pragma once

#include "context/panel.h"
#include "scripting/iscript.h"
#include "scripting/scripting.h"

namespace UI
{
	class InspectorPanel : public IUIPanel
	{
	public:
		InspectorPanel() = default;

		void Draw() override;

	private:
		struct PropertyDrawData
		{
			rttr::property property;
			rttr::variant value;
			UIData ui;
		};

		void DrawAllComponents(GameObject* currentGameOject);

		void DrawComponent(IComponent* component, const std::string &name, unsigned int textureID, const std::string& scriptTypeName = "");
		void DrawTransform(GameObject* currentGameOject);
		void DrawMaterial(GameObject* currentGameOject, unsigned int textureID);
		void DrawTextureSlot(bool& useTexture, Texture& textureObj, unsigned int placeholderImg, const char* payloadType);

		void DrawProperty(IComponent* component, const ComponentPropertyDescriptor& descriptor);
		void DrawScriptProperty(Scripting::IScript* script, const Scripting::ScriptPropertyDescriptor& descriptor);

		void DrawComponentEntry(const char* name, const char* icon, const std::string& search, const std::function<void()> &onAdd);
		void DrawAddComponentPopUp(GameObject* current);

		std::string GetScriptRegistryName(const Scripting::IScript* script);

		GameObject* currentGameObject = nullptr;
	};
}
