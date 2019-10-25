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

        bool onEvent(Ref<Event> event) override;

        void begin();
        void end();

    private:
        void setupGuiLibrary();
        void drawViewport();

        u32 m_viewportHandle;

        u32 m_currentEntity = -1;
    };
}
