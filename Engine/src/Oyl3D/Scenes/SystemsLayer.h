#pragma once

#include "Layer.h"

namespace oyl::internal
{
    class SystemsLayer : public Layer
    {
        OYL_CTOR(SystemsLayer, Layer)

        void onEnter() override;
        void onExit() override;

        void onUpdate() override;
        void onGuiRender() override;
        bool onEvent(Ref<Event> event) override;
    };
}