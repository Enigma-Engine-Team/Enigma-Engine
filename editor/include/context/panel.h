#pragma once

#include "imgui.h"
#include "engine/engine.h"
#include "ui_utils.h"
#include "style_manager.h"

namespace UI 
{
	class IUIPanel
	{
	public:
		virtual void Draw() = 0;
		virtual ~IUIPanel() = default;
	};
}