#pragma once

#include "context/panel.h"

namespace UI
{
	class DebugConsolePanel : public IUIPanel
	{
	public:
		DebugConsolePanel();

		void Draw() override;

	private:

		bool autoScroll = false;
	};
}