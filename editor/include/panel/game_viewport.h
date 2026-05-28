#pragma once

#include "context/panel.h"
#include "viewport.h"

namespace UI
{
	class GameViewportPanel : public ViewportPanel
	{
	public:
		GameViewportPanel();

		void Draw() override;
	};
}