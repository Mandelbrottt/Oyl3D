#include "oylpch.h"
#include "SceneToFile.h"

#include "ECS/Component.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Scenes/Scene.h"

// TODO: Store materials separately from entities

namespace oyl::internal
{
    static void saveTransforms(json& j, entt::registry& registry);
    static void saveParents(json& j, entt::registry& registry);
    static void saveRenderables(json& j, entt::registry& registry);
    static void saveColliders(json& j, entt::registry& registry);
    static void saveRigidBodies(json& j, entt::registry& registry);

    void saveSceneToFile(const Scene& scene)
    {
        json sceneJson;

        saveTransforms(sceneJson, *scene.m_registry);
        saveParents(sceneJson, *scene.m_registry);
        saveRenderables(sceneJson, *scene.m_registry);
        saveColliders(sceneJson, *scene.m_registry);
        saveRigidBodies(sceneJson, *scene.m_registry);

        std::ofstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
        sceneFile << std::setw(4) << sceneJson;
    }

    static void loadTransform(const json& j, entt::registry& registry, entt::entity entity);
    static void loadParent(const json& j, entt::registry& registry, entt::entity entity);
    static void loadRenderable(const json& j, entt::registry& registry, entt::entity entity);
    static void loadCollider(const json& j, entt::registry& registry, entt::entity entity);
    static void loadRigidBody(const json& j, entt::registry& registry, entt::entity entity);

    void loadSceneFromFile(Scene& scene)
    {
        using component::SceneObject;

        std::ifstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
        if (!sceneFile)
            return;

        json sceneJson;
        sceneFile >> sceneJson;
        sceneFile.close();

        auto view = scene.m_registry->view<SceneObject>();
        for (auto& [key, value] : sceneJson.items())
        {
            entt::entity entityToLoad = entt::null;

            for (auto entity : view)
            {
                SceneObject& so = view.get(entity);
                if (so.name == key)
                {
                    entityToLoad = entity;
                    break;
                }
            }
            if (entityToLoad == entt::null)
                entityToLoad = scene.m_registry->create();

            auto& so = scene.m_registry->get_or_assign<SceneObject>(entityToLoad);
            so.name = key;
            
            if (value.find("Transform") != value.end())
                loadTransform(value["Transform"], *scene.m_registry, entityToLoad);

            if (value.find("Renderable") != value.end())
                loadRenderable(value["Renderable"], *scene.m_registry, entityToLoad);

            if (value.find("Collider") != value.end())
                loadCollider(value["Collider"], *scene.m_registry, entityToLoad);

            if (value.find("RigidBody") != value.end())
                loadCollider(value["RigidBody"], *scene.m_registry, entityToLoad);
        }

        for (auto& [key, value] : sceneJson.items())
        {
            for (auto entity : view)
            {
                SceneObject& so = view.get(entity);
                if (so.name == key)
                {
                    if (value.find("Parent") != value.end() &&
                        value["Parent"]["Name"].get<std::string>() != key)
                        loadParent(value["Parent"], *scene.m_registry, entity);

                    break;
                }
            }
        }
    }

    void saveSceneBackupToFile(const Scene& scene)
    {
        json sceneBackup;

        std::ifstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
        if (sceneFile)
        {
            sceneFile >> sceneBackup;

            std::ofstream sceneBackupFile("res/scenes/" + scene.m_name + ".oylscene.backup");
            sceneBackupFile << std::setw(4) << sceneBackup;
        }
    }

    void saveTransforms(json& j, entt::registry& registry)
    {
        using component::SceneObject;
        using component::Transform;

        auto view = registry.view<SceneObject, Transform>();
        for (auto entity : view)
        {
            auto& so = view.get<SceneObject>(entity);
            auto& t = view.get<Transform>(entity);

            auto& jName = j[so.name]["Transform"];

            auto& jPos = jName["Position"];
            jPos["X"] = t.getPositionX();
            jPos["Y"] = t.getPositionY();
            jPos["Z"] = t.getPositionZ();

            auto& jRot = jName["Rotation"];
            jRot["X"] = t.getRotationEulerX();
            jRot["Y"] = t.getRotationEulerY();
            jRot["Z"] = t.getRotationEulerZ();

            auto& jSca = jName["Scale"];
            jSca["X"] = t.getScaleX();
            jSca["Y"] = t.getScaleY();
            jSca["Z"] = t.getScaleZ();
        }
    }

    void saveParents(json& j, entt::registry& registry)
    {
        using component::SceneObject;
        using component::Parent;

        auto view = registry.view<SceneObject, Parent>();
        for (auto entity : view)
        {
            auto& so = view.get<SceneObject>(entity);
            auto& pa = view.get<Parent>(entity);

            if (registry.valid(pa.parent))
                j[so.name]["Parent"]["Name"] = registry.get<SceneObject>(pa.parent).name;
        }
    }

    static void saveRenderables(json& j, entt::registry& registry)
    {
        using component::SceneObject;
        using component::Renderable;

        auto view = registry.view<SceneObject, Renderable>();
        for (auto entity : view)
        {
            auto& so = view.get<SceneObject>(entity);
            auto& re = view.get<Renderable>(entity);

            auto& jName = j[so.name]["Renderable"];

            if (re.mesh)
            {
                auto& jMesh = jName["Mesh"];

                jMesh["Alias"]    = Mesh::getAlias(re.mesh);
                jMesh["FilePath"] = re.mesh->getFilePath();
            }

            if (re.material)
            {
                auto& jMat = jName["Material"];
                jMat["Alias"] = Material::getAlias(re.material);

                if (re.material->shader)
                {
                    auto& jSha    = jMat["Shader"];
                    jSha["Alias"] = Shader::getAlias(re.material->shader);

                    for (const auto& info : re.material->shader->getShaderInfos())
                    {
                        const char* type = nullptr;

                        switch (info.type)
                        {
                            case VertexShader:
                                type = "Vertex";
                                break;
                            case GeometryShader:
                                type = "Geometry";
                                break;
                            case TessControlShader:
                                type = "TessControl";
                                break;
                            case TessEvaluationShader:
                                type = "TessEvaluation";
                                break;
                            case PixelShader:
                                type = "Pixel";
                                break;
                            default:
                                OYL_ASSERT(false, "Invalid Shader Type!");
                        }

                        if (type)
                        {
                            jSha[type] = info.filename;
                        }
                    }
                }

                if (re.material->albedoMap)
                {
                    auto& jTex = jMat["Albedo"];

                    jTex["Alias"] = Texture2D::getAlias(re.material->albedoMap);
                    jTex["FilePath"] = re.material->albedoMap->getFilePath();
                }

                if (re.material->specularMap)
                {
                    auto& jTex = jMat["Specular"];

                    jTex["Alias"] = Texture2D::getAlias(re.material->specularMap);
                    jTex["FilePath"] = re.material->specularMap->getFilePath();
                }

                //if (re.material->normalMap)
                //{
                //    auto& jTex = jMat["albedoMap"];

                //    jTex["Alias"] = Texture2D::getAlias(re.material->normalMap);
                //    jTex["FilePath"] = re.material->normalMap->getFilePath();
                //}
            }
        }
    }

    static void saveColliders(json& j, entt::registry& registry)
    {
        
    }
    
    static void saveRigidBodies(json& j, entt::registry& registry)
    {
        
    }

    void loadTransform(const json& j, entt::registry& registry, entt::entity entity)
    {
        using component::Transform;

        auto& t = registry.get_or_assign<Transform>(entity);

        t.setPositionX(j["Position"]["X"].get<float>());
        t.setPositionY(j["Position"]["Y"].get<float>());
        t.setPositionZ(j["Position"]["Z"].get<float>());

        t.setRotationEulerX(j["Rotation"]["X"].get<float>());
        t.setRotationEulerY(j["Rotation"]["Y"].get<float>());
        t.setRotationEulerZ(j["Rotation"]["Z"].get<float>());

        t.setScaleX(j["Scale"]["X"].get<float>());
        t.setScaleY(j["Scale"]["Y"].get<float>());
        t.setScaleZ(j["Scale"]["Z"].get<float>());
    }
    
    void loadParent(const json& j, entt::registry& registry, entt::entity entity)
    {
        using component::SceneObject;
        using component::Parent;
        
        std::string name = j["Name"].get<std::string>();
        auto view = registry.view<SceneObject>();
        for (auto e : view)
        {
            if (view.get(e).name == name)
            {
                auto& p = registry.get_or_assign<Parent>(entity);
                p.parent = e;
            }
        }   
    }

    static void loadRenderable(const json& j, entt::registry& registry, entt::entity entity)
    {
        using component::Renderable;

        auto& re = registry.get_or_assign<Renderable>(entity);

        if (j.find("Mesh") != j.end())
        {
            auto alias    = j["Mesh"]["Alias"].get<std::string>();
            auto filePath = j["Mesh"]["FilePath"].get<std::string>();
            if (Mesh::exists(alias))
            {
                re.mesh = Mesh::get(alias);
                
                if (re.mesh->getFilePath() != filePath)
                    re.mesh = Mesh::cache(filePath);
            }
            else
                re.mesh = Mesh::cache(filePath, alias);
        }
        else re.mesh = Mesh::get(INVALID_ALIAS);

        if (j.find("Material") != j.end())
        {
            auto& jMat = j["Material"];

            if (!Material::exists(jMat["Alias"].get<std::string>()))
                re.material = Material::cache(Material::create(), jMat["Alias"].get<std::string>());
            else
                re.material = Material::get(jMat["Alias"].get<std::string>());

            if (jMat.find("Shader") != jMat.end())
            {
                auto& jSha  = jMat["Shader"];
                auto  alias = jSha["Alias"].get<std::string>();

                if (Shader::exists(alias))
                    re.material->shader = Shader::get(alias);
                else
                {
                    std::vector<ShaderInfo> infos;
                    infos.reserve(5);
                    
                    if (jSha.find("Vertex") != jSha.end())
                        infos.push_back({ VertexShader, jSha["Vertex"].get<std::string>() });

                    if (jSha.find("Geometry") != jSha.end())
                        infos.push_back({ GeometryShader, jSha["Geometry"].get<std::string>() });

                    if (jSha.find("TessControl") != jSha.end())
                        infos.push_back({ TessControlShader, jSha["TessControl"].get<std::string>() });

                    if (jSha.find("TessEvaluation") != jSha.end())
                        infos.push_back({ TessEvaluationShader, jSha["TessEvaluation"].get<std::string>() });

                    if (jSha.find("Pixel") != jSha.end())
                        infos.push_back({ PixelShader, jSha["Pixel"].get<std::string>() });

                    Shader::cache(infos, alias);
                }
            } else re.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);

            if (jMat.find("Albedo") != jMat.end())
            {
                auto alias = jMat["Albedo"]["Alias"].get<std::string>();
                auto filePath = jMat["Albedo"]["FilePath"].get<std::string>();
                if (Texture2D::exists(alias))
                {
                    re.material->albedoMap = Texture2D::get(alias);

                    if (re.material->albedoMap->getFilePath() != filePath)
                        re.material->albedoMap = Texture2D::cache(filePath);
                } else
                    re.material->albedoMap = Texture2D::cache(filePath, alias);
            } else re.material->albedoMap = Texture2D::get(WHITE_TEXTURE_ALIAS);

            if (jMat.find("Specular") != jMat.end())
            {
                auto alias = jMat["Specular"]["Alias"].get<std::string>();
                auto filePath = jMat["Specular"]["FilePath"].get<std::string>();
                if (Texture2D::exists(alias))
                {
                    re.material->specularMap = Texture2D::get(alias);

                    if (re.material->specularMap->getFilePath() != filePath)
                        re.material->specularMap = Texture2D::cache(filePath);
                } else
                    re.material->specularMap = Texture2D::cache(filePath, alias);
            } else re.material->specularMap = Texture2D::get(WHITE_TEXTURE_ALIAS);
        }
    }

    static void loadCollider(const json& j, entt::registry& registry, entt::entity entity)
    {
        
    }
    
    static void loadRigidBody(const json& j, entt::registry& registry, entt::entity entity)
    {
        
    }
}
