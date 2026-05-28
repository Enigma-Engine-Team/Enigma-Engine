#pragma once

#include "context/panel.h"

namespace UI
{
	class SceneHierarchyPanel : public IUIPanel
	{
	public:
		SceneHierarchyPanel();

		void Draw() override;

	private:

		void RenderHierarchy(Scene* currentScene) const;

		unsigned int objectID = 0;
		unsigned int sceneID = 0;
	};
}