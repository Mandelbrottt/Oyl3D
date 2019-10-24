#pragma once

#include "Oyl3D/Scenes/Layer.h"

// TODO: Turn off mouse input when cursor is disabled

namespace oyl
{
    class GuiLayer : public Layer
    {
    public:
        OYL_CTOR(GuiLayer, Layer)

        void onEnter() override;
        void onExit() override;
        void onGuiRender(Timestep dt) override;

        void begin();
        void end();
    };
}
