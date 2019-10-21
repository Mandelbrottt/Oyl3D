#pragma once

#include <Oyl3D/Scenes/Layer.h>

class SandboxLayer : public oyl::Layer
{
public:
    OYL_CREATE_FUNC(SandboxLayer)

    explicit SandboxLayer();

    virtual void onAttach() override;

    virtual void onUpdate(oyl::Timestep dt) override;

    virtual void onGuiRender() override;

    virtual bool onEvent(oyl::Ref<oyl::Event> event) override;
};
