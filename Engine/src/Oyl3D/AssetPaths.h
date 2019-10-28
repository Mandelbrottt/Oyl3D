#pragma once

#include "Oyl3D/Typedefs.h"

namespace oyl
{
    // vvv Universal vvv //

    const CacheAlias INVALID_ALIAS = "_invalid";
    
    // ^^^ Universal ^^^ //
    // vvv Shaders vvv //
    
    const std::string ENGINE_RES = "../Engine/res/";

    const CacheAlias  LIGHTING_SHADER_ALIAS         = "_lighting";
    const std::string LIGHTING_SHADER_VERTEX_PATH   = "shaders/lighting.vert";
    const std::string LIGHTING_SHADER_FRAGMENT_PATH = "shaders/lighting.frag";
    
    const CacheAlias  TEXTURE_SHADER_ALIAS         = "_texture";
    const std::string TEXTURE_SHADER_VERTEX_PATH   = "shaders/meshShader.vert";
    const std::string TEXTURE_SHADER_FRAGMENT_PATH = "shaders/meshShader.frag";

    // ^^^ Shaders ^^^ //
    // vvv Meshes vvv //

    const std::string INVALID_MESH_PATH = "models/invalid.obj";

    const CacheAlias CUBE_MESH_ALIAS = "_cube";
    const std::string CUBE_MESH_PATH = "models/cube.obj";

    const CacheAlias MONKEY_MESH_ALIAS = "_monkey";
    const std::string MONKEY_MESH_PATH = "models/monkey.obj";

    // ^^^ Shaders ^^^ //
    // vvv Textures vvv //
    
    const std::string INVALID_TEXTURE_PATH = "textures/invalid.png";

    const CacheAlias WHITE_TEXTURE_ALIAS = "_white";
    const std::string WHITE_TEXTURE_PATH = "textures/white.png";

    const CacheAlias UV_TEXTURE_ALIAS = "_uvgrid";
    const std::string UV_TEXTURE_PATH = "textures/uvgrid.png";

    // ^^^ Textures ^^^ //
}
