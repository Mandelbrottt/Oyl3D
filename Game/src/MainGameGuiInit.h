#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class MainGameGuiInitLayer : public oyl::Layer
{
public:
    OYL_CTOR(MainGameGuiInitLayer, Layer)

    virtual void onEnter() override;

    virtual void onUpdate() override;

    virtual void onGuiRender() override;
};
