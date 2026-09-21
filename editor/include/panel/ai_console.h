#pragma once

#include "context/panel.h"

namespace UI
{
    class AiConsolePanel : public IUIPanel
    {
    public:
        AiConsolePanel();

        void Draw() override;

    private:

        bool autoScroll = false;
    };
}