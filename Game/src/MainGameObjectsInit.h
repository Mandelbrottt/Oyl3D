#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

class MainGameObjectsInitLayer : public oyl::Layer
{
public:
    OYL_CTOR(MainGameObjectsInitLayer, Layer)

    virtual void onEnter() override;

    virtual void onUpdate() override;

    virtual void onGuiRender() override;
};
