#pragma once

namespace oyl
{
    // vvv Universal vvv //

    const CacheAlias INVALID_ALIAS = "Oyl Invalid";
    
    // ^^^ Universal ^^^ //
    // vvv Shaders vvv //
    
    const std::string ENGINE_RES = "../Engine/res/";

    const CacheAlias  LIGHTING_SHADER_ALIAS         = "Oyl Lighting";
    const std::string LIGHTING_SHADER_VERTEX_PATH   = "shaders/lighting.vert";
    const std::string LIGHTING_SHADER_FRAGMENT_PATH = "shaders/lighting.frag";
    
    const CacheAlias  TEXTURE_SHADER_ALIAS         = "Oyl Texture";
    const std::string TEXTURE_SHADER_VERTEX_PATH   = "shaders/meshShader.vert";
    const std::string TEXTURE_SHADER_FRAGMENT_PATH = "shaders/meshShader.frag";

    const CacheAlias  SKYBOX_SHADER_ALIAS         = "Oyl Skybox";
    const std::string SKYBOX_SHADER_VERTEX_PATH   = "shaders/skybox.vert";
    const std::string SKYBOX_SHADER_FRAGMENT_PATH = "shaders/skybox.frag";

    // ^^^ Shaders ^^^ //
    // vvv Meshes vvv //

    const std::string INVALID_MESH_PATH = "models/invalid.obj";

    const CacheAlias  CUBE_MESH_ALIAS = "Oyl Cube";
    const std::string CUBE_MESH_PATH  = "models/cube.obj";

    const CacheAlias  MONKEY_MESH_ALIAS = "Oyl Monkey";
    const std::string MONKEY_MESH_PATH  = "models/monkey.obj";

    // ^^^ Shaders ^^^ //
    // vvv Textures vvv //
    
    const std::string INVALID_TEXTURE_PATH = "textures/invalid.png";

    const CacheAlias  WHITE_TEXTURE_ALIAS = "Oyl White";
    const std::string WHITE_TEXTURE_PATH  = "textures/white.png";
    
    const CacheAlias  BLACK_TEXTURE_ALIAS = "Oyl Black";
    const std::string BLACK_TEXTURE_PATH  = "textures/white.png";

    const CacheAlias  UV_TEXTURE_ALIAS = "Oyl UV-Grid";
    const std::string UV_TEXTURE_PATH  = "textures/uvgrid.png";
    
    const CacheAlias  DEFAULT_SKYBOX_ALIAS = "Oyl Default Skybox";
    const std::string DEFAULT_SKYBOX_PATH  = "textures/sea/sea.jpg";

    // ^^^ Textures ^^^ //
}
