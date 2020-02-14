#pragma once

#include "Oyl3D/oylpch.h"

#include "Oyl3D/Graphics/Material.h"
#include "Oyl3D/Graphics/Model.h"
#include "Oyl3D/Graphics/Texture.h"

namespace oyl::component
{
    struct Renderable
    {
        Ref<Model>     model;
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