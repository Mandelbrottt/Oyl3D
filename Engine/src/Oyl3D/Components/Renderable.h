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
    
        bool enabled = true;
    };
    
    struct GuiRenderable
    {
        Ref<Texture2D> texture;
    
        bool enabled = true;
    };
}