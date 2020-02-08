#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class Mesh;
    class Material;
    class Texture2D;    
}

namespace oyl::component
{
    struct Renderable
    {
        Ref<Mesh>      mesh;
        Ref<Material>  material;

        u32 cullingMask = 0x0F;
    
        bool enabled        = true;
        bool castShadows    = true;
        //bool receiveShadows = true;
    };
    
    struct GuiRenderable
    {
        Ref<Texture2D> texture;

        u32 cullingMask = 0x0F;

        glm::vec2 lowerClipping = glm::vec2(0.0f);
        glm::vec2 upperClipping = glm::vec2(1.0f);
    
        bool enabled = true;
    };
}