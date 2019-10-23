#pragma once

#include "Oyl3D/Scenes/Layer.h"

namespace oyl
{
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        virtual ~ImGuiLayer();

        virtual void onEnter() override;
        virtual void onExit() override;
        virtual void onGuiRender(Timestep dt) override;

        void begin();
        void end();
    private:
        float m_time = 0;
    };
}
