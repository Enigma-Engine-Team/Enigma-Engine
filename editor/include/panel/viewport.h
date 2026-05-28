#pragma once

#include "context/panel.h"
#include <src/ImGuizmo.h>

namespace UI
{
	class ViewportPanel : public IUIPanel
	{
	public:
		ViewportPanel();

		void Draw() override;

	protected:

		enum EGuizmoMode
		{
			TRANSLATE = ImGuizmo::TRANSLATE,
			ROTATE = ImGuizmo::ROTATE,
			SCALE = ImGuizmo::SCALE
		};

		void GetGuizmoInputs();
		EGuizmoMode currentGizmoMode = EGuizmoMode::TRANSLATE;

	private:

		void AddMenuBarButton();
		void ComputeDragAndDrop(Scene* scene);
		void ComputeGizmos(Scene* scene, Math::Vector2D windowPos, Math::Vector2D windowSize) const;
		bool canPickObject = false;
	};
}