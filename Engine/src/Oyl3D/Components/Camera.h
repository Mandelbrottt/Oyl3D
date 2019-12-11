#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class TextureCubeMap;
}

namespace oyl::component
{
    struct PlayerCamera
    {
        i32 player = -1;

        glm::mat4 projection = glm::mat4(1.0f);

        Ref<TextureCubeMap> skybox;
    };
}