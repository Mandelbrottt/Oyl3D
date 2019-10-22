#pragma once

#include <Oyl3D.h>

class SandboxLayer : public oyl::Layer
{
public:
    OYL_CTOR(SandboxLayer, Layer)

    virtual void onAttach() override;

    virtual void onUpdate(oyl::Timestep dt) override;

    virtual void onGuiRender() override;

    virtual bool onEvent(oyl::Ref<oyl::Event> event) override;
};
