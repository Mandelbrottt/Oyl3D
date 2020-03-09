#pragma once

namespace oyl
{
    // vvv Universal vvv //

    const CacheAlias INVALID_ALIAS = "Oyl Invalid";
    
    // ^^^ Universal ^^^ //
    // vvv Shaders vvv //
    
    const std::string ENGINE_RES = "../Engine/res/";

    const CacheAlias  FORWARD_STATIC_SHADER_ALIAS         = "Oyl Forward Static";
    const std::string FORWARD_STATIC_SHADER_VERTEX_PATH   = "shaders/Lighting/Static_Normal_Shadow.vert";
    const std::string FORWARD_STATIC_SHADER_FRAGMENT_PATH = "shaders/Lighting/Forward_Normal_Shadow.frag";

    const CacheAlias  FORWARD_VERTEX_SHADER_ALIAS         = "Oyl Forward Vertex";
    const std::string FORWARD_VERTEX_SHADER_VERTEX_PATH   = "shaders/Lighting/Vertex_Normal_Shadow.vert";
    const std::string FORWARD_VERTEX_SHADER_FRAGMENT_PATH = "shaders/Lighting/Forward_Normal_Shadow.frag";

    const CacheAlias  FORWARD_SKELETAL_SHADER_ALIAS         = "Oyl Forward Skeletal";
    const std::string FORWARD_SKELETAL_SHADER_VERTEX_PATH   = "shaders/Lighting/Skeletal_Normal_Shadow.vert";
    const std::string FORWARD_SKELETAL_SHADER_FRAGMENT_PATH = "shaders/Lighting/Forward_Normal_Shadow.frag";

    const CacheAlias  DEFERRED_STATIC_PRE_SHADER_ALIAS         = "Oyl Deferred Static";
    const std::string DEFERRED_STATIC_PRE_SHADER_VERTEX_PATH   = "shaders/Lighting/Static_Normal_Shadow.vert";
    const std::string DEFERRED_STATIC_PRE_SHADER_FRAGMENT_PATH = "shaders/Lighting/Deferred_Pre_Normal_Shadow.frag";

    const CacheAlias  DEFERRED_VERTEX_PRE_SHADER_ALIAS         = "Oyl Deferred Vertex";
    const std::string DEFERRED_VERTEX_PRE_SHADER_VERTEX_PATH   = "shaders/Lighting/Vertex_Normal_Shadow.vert";
    const std::string DEFERRED_VERTEX_PRE_SHADER_FRAGMENT_PATH = "shaders/Lighting/Deferred_Pre_Normal_Shadow.frag";

    const CacheAlias  DEFERRED_SKELETAL_PRE_SHADER_ALIAS         = "Oyl Deferred Skeletal";
    const std::string DEFERRED_SKELETAL_PRE_SHADER_VERTEX_PATH   = "shaders/Lighting/Skeletal_Normal_Shadow.vert";
    const std::string DEFERRED_SKELETAL_PRE_SHADER_FRAGMENT_PATH = "shaders/Lighting/Deferred_Pre_Normal_Shadow.frag";

    const std::string DEFERRED_POST_FINAL_SHADER_VERTEX_PATH   = "shaders/ScreenSpace/FBO_Passthrough.vert";
    const std::string DEFERRED_POST_FINAL_SHADER_FRAGMENT_PATH = "shaders/Lighting/Deferred_Post_Final_Normal_Shadow.frag";
    
    const std::string DEFERRED_POST_POINT_LIGHT_SHADER_VERTEX_PATH   = "shaders/Lighting/Deferred_Post_Volume.vert";
    const std::string DEFERRED_POST_POINT_LIGHT_SHADER_FRAGMENT_PATH = "shaders/Lighting/Deferred_Post_Light_Normal_Shadow.frag";
    
    const std::string SKYBOX_SHADER_PATH         = "shaders/Skybox.oylshader";

    const std::string HDR_SHADER_VERTEX_PATH     = "shaders/ScreenSpace/FBO_Passthrough.vert";
    const std::string HDR_SHADER_FRAGMENT_PATH   = "shaders/ScreenSpace/HDR_Post.frag";

    // ^^^ Shaders ^^^ //
    // vvv Meshes vvv //

    const std::string INVALID_MODEL_PATH = "models/invalid.obj";

    const CacheAlias  CUBE_MODEL_ALIAS = "Oyl Cube";
    const std::string CUBE_MODEL_PATH  = "models/cube.obj";

    const CacheAlias  MONKEY_MODEL_ALIAS = "Oyl Monkey";
    const std::string MONKEY_MODEL_PATH  = "models/monkey.obj";

    const CacheAlias  SPHERE_MODEL_ALIAS = "Oyl Sphere";
    const std::string SPHERE_MODEL_PATH  = "models/sphere.obj";

    // ^^^ Shaders ^^^ //
    // vvv Textures vvv //
    
    const std::string INVALID_TEXTURE_PATH = "textures/invalid.png";

    const CacheAlias  WHITE_TEXTURE_ALIAS = "Oyl White";
    const std::string WHITE_TEXTURE_PATH  = "textures/white.png";
    
    const CacheAlias  BLACK_TEXTURE_ALIAS = "Oyl Black";
    const std::string BLACK_TEXTURE_PATH  = "textures/black.png";

    const CacheAlias  UV_TEXTURE_ALIAS = "Oyl UV-Grid";
    const std::string UV_TEXTURE_PATH  = "textures/uvgrid.png";

    const CacheAlias  DEFAULT_NORMAL_TEXTURE_ALIAS = "Oyl Default Normal";
    const std::string DEFAULT_NORMAL_TEXTURE_PATH = "textures/default_normal.png";
    
    const CacheAlias  DEFAULT_SKYBOX_ALIAS = "Oyl Default Skybox";
    const std::string DEFAULT_SKYBOX_PATH  = "textures/sea/sea.jpg";

    // ^^^ Textures ^^^ //
}
