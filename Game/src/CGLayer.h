#pragma once

#include <Oyl3D.h>

class CGLayer : public oyl::Layer
{
    OYL_CTOR(CGLayer, oyl::Layer)

public:
    void onEnter() override;
    void onExit() override;
    void onUpdate() override;
    void onGuiRender() override;
    bool onEvent(const oyl::Event& event) override;;
};