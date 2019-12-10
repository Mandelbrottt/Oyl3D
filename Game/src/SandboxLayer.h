#pragma once

#include <Oyl3D.h>

class SandboxLayer : public oyl::Layer
{
public:
    OYL_CTOR(SandboxLayer, Layer)

    virtual void onEnter() override;

    virtual void onUpdate() override;

    virtual void onGuiRender() override;

    float forceSpeed = 1.0f;
};
