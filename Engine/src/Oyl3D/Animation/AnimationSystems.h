#pragma once
#include "Scenes/System.h"

namespace oyl::internal
{
    class AnimationSystem : public System
    {
        OYL_CTOR(AnimationSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;
    };

    class SkeletalAnimationSystem : public System
    {
        OYL_CTOR(SkeletalAnimationSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;
    };
}
