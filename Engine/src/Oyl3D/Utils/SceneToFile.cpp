#include "oylpch.h"
#include "SceneToFile.h"

#include "Components/Collidable.h"
#include "Components/Misc.h"
#include "Components/Renderable.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Scenes/Scene.h"

// TODO: Store materials separately from entities

namespace oyl::internal
{
    //static void saveTransforms(json& j, entt::registry& registry);
    //static void saveParents(json& j, entt::registry& registry);
    //static void saveRenderables(json& j, entt::registry& registry);
    //static void saveColliders(json& j, entt::registry& registry);
    //static void saveRigidBodies(json& j, entt::registry& registry);

    //void saveSceneToFile(const Scene& scene)
    //{
    //    json sceneJson;

    //    saveTransforms(sceneJson, *scene.m_registry);
    //    saveParents(sceneJson, *scene.m_registry);
    //    saveRenderables(sceneJson, *scene.m_registry);
    //    saveColliders(sceneJson, *scene.m_registry);
    //    saveRigidBodies(sceneJson, *scene.m_registry);

    //    std::ofstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
    //    sceneFile << std::setw(4) << sceneJson;
    //}

    //static void loadTransform(const json& j, entt::registry& registry, entt::entity entity);
    //static void loadParent(const json& j, entt::registry& registry, entt::entity entity);
    //static void loadRenderable(const json& j, entt::registry& registry, entt::entity entity);
    //static void loadCollider(const json& j, entt::registry& registry, entt::entity entity);
    //static void loadRigidBody(const json& j, entt::registry& registry, entt::entity entity);

    //void loadSceneFromFile(Scene& scene)
    //{
    //    using component::EntityInfo;

    //    std::ifstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
    //    if (!sceneFile)
    //        return;

    //    json sceneJson;
    //    sceneFile >> sceneJson;
    //    sceneFile.close();

    //    auto view = scene.m_registry->view<EntityInfo>();
    //    for (auto& [key, value] : sceneJson.items())
    //    {
    //        entt::entity entityToLoad = entt::null;

    //        for (auto entity : view)
    //        {
    //            EntityInfo& so = view.get(entity);
    //            if (so.name == key)
    //            {
    //                entityToLoad = entity;
    //                break;
    //            }
    //        }
    //        if (entityToLoad == entt::null)
    //            entityToLoad = scene.m_registry->create();

    //        auto& so = scene.m_registry->get_or_assign<EntityInfo>(entityToLoad);
    //        so.name = key;
    //        
    //        if (value.find("Transform") != value.end())
    //            loadTransform(value["Transform"], *scene.m_registry, entityToLoad);

    //        if (value.find("Renderable") != value.end())
    //            loadRenderable(value["Renderable"], *scene.m_registry, entityToLoad);

    //        if (value.find("Collider") != value.end())
    //            loadCollider(value["Collider"], *scene.m_registry, entityToLoad);

    //        if (value.find("RigidBody") != value.end())
    //            loadCollider(value["RigidBody"], *scene.m_registry, entityToLoad);
    //    }

    //    for (auto& [key, value] : sceneJson.items())
    //    {
    //        for (auto entity : view)
    //        {
    //            EntityInfo& so = view.get(entity);
    //            if (so.name == key)
    //            {
    //                if (value.find("Parent") != value.end() &&
    //                    value["Parent"]["Name"].get<std::string>() != key)
    //                    loadParent(value["Parent"], *scene.m_registry, entity);

    //                break;
    //            }
    //        }
    //    }
    //}

    //void saveSceneBackupToFile(const Scene& scene)
    //{
    //    json sceneBackup;

    //    std::ifstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
    //    if (sceneFile)
    //    {
    //        sceneFile >> sceneBackup;

    //        std::ofstream sceneBackupFile("res/scenes/" + scene.m_name + ".oylscene.backup");
    //        sceneBackupFile << std::setw(4) << sceneBackup;
    //    }
    //}

    void saveTransform(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::Transform;

        auto& t = registry.get_or_assign<Transform>(entity);

        {   
            auto& jPos = j["Position"];
            glm::vec3 position = t.getPosition();
            jPos["X"] = position.x;
            jPos["Y"] = position.y;
            jPos["Z"] = position.z;
        }
        {
            auto& jRot = j["Rotation"];
            glm::quat rotation = t.getRotation();
            jRot["qW"] = rotation.w;
            jRot["qX"] = rotation.x;
            jRot["qY"] = rotation.y;
            jRot["qZ"] = rotation.z;
        }
        {
            auto& jSca = j["Scale"];
            glm::vec3 scale = t.getScale();
            jSca["X"] = scale.x;
            jSca["Y"] = scale.y;
            jSca["Z"] = scale.z;
        }
        {
            auto& jMir = j["Mirror"];
            glm::bvec3 mirror = t.getMirror();
            jMir["X"] = mirror.x;
            jMir["Y"] = mirror.y;
            jMir["Z"] = mirror.z;
        }
    }

    void saveParent(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::EntityInfo;
        using component::Parent;

        if (!registry.has<Parent>(entity))
            return;
        
        auto& pa = registry.get<Parent>(entity);

        if (registry.valid(pa.parent))
            j["Name"] = registry.get<EntityInfo>(pa.parent).name;
        
    }

    static void saveRenderable(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::EntityInfo;
        using component::Renderable;

        if (!registry.has<Renderable>(entity))
            return;
        
        auto& re = registry.get<Renderable>(entity);

        j["Enabled"] = re.enabled;
        
        if (re.mesh)
        {
            auto& jMesh = j["Mesh"];
            jMesh["Filepath"] = re.mesh->getFilePath();
        }
        else
        {
            j["Mesh"]["Filepath"];
        }

        if (false && re.material)
        {
            auto& jMat = j["Material"];
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
                        case Shader::Vertex:
                            type = "Vertex";
                            break;
                        case Shader::Geometry:
                            type = "Geometry";
                            break;
                        case Shader::TessControl:
                            type = "TessControl";
                            break;
                        case Shader::TessEvaluation:
                            type = "TessEvaluation";
                            break;
                        case Shader::Pixel:
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

    static void saveColliders(json& j, entt::registry& registry)
    {
        
    }
    
    static void saveRigidBodies(json& j, entt::registry& registry)
    {
        
    }

    void loadTransform(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::Transform;

        auto& t = registry.get_or_assign<Transform>(entity);

        {
            const auto posIt = j.find("Position");

            if (posIt != j.end()     &&
                posIt->contains("X") &&
                posIt->contains("Y") &&
                posIt->contains("Z"))
            {
                glm::vec3 position = {
                    posIt->at("X").get<float>(),
                    posIt->at("Y").get<float>(),
                    posIt->at("Z").get<float>(),
                };
                
                t.setPosition(position);
            }
        }
        {
            const auto rotIt = j.find("Rotation");

            if (rotIt != j.end()     &&
                rotIt->contains("qW") &&
                rotIt->contains("qX") &&
                rotIt->contains("qY") &&
                rotIt->contains("qZ"))
            {
                glm::quat rotation = {
                    rotIt->at("qW").get<float>(),
                    rotIt->at("qX").get<float>(),
                    rotIt->at("qY").get<float>(),
                    rotIt->at("qZ").get<float>(),
                };

                t.setRotation(rotation);
            }
        }
        {
            const auto scaIt = j.find("Scale");

            if (scaIt != j.end()     &&
                scaIt->contains("X") &&
                scaIt->contains("Y") &&
                scaIt->contains("Z"))
            {
                glm::vec3 scale = {
                    scaIt->at("X").get<float>(),
                    scaIt->at("Y").get<float>(),
                    scaIt->at("Z").get<float>(),
                };

                t.setScale(scale);
            }
        }
        {
            const auto mirIt = j.find("Mirror");

            if (mirIt != j.end() &&
                mirIt->contains("X") &&
                mirIt->contains("Y") &&
                mirIt->contains("Z"))
            {
                glm::bvec3 mirror = {
                    mirIt->at("X").get<bool>(),
                    mirIt->at("Y").get<bool>(),
                    mirIt->at("Z").get<bool>(),
                };

                t.setMirror(mirror);
            }
        }
    }
    
    void loadParent(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::EntityInfo;
        using component::Parent;
        
        const auto it = j.find("Name");
        if (it != j.end() && !it->is_null() && it->is_string())
        {
            std::string name = it->get<std::string>();
            auto view = registry.view<EntityInfo>();
            for (auto e : view)
            {
                if (view.get(e).name == name)
                {
                    auto& p = registry.get_or_assign<Parent>(entity);
                    p.parent = e;
                }
            }
        }   
    }

    static void loadRenderable(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::Renderable;

        auto& re = registry.get_or_assign<Renderable>(entity);

        if (j.contains("Enabled"))
            re.enabled = j["Enabled"].get<bool>();

        if (auto it = j.find("Mesh"); it != j.end() && !it->is_null())
        {
            if (auto fpIt = it->find("FilePath"); fpIt != it->end())
            {
                auto filePath = fpIt->get<std::string>();
                   
                if (re.mesh && re.mesh->getFilePath() != filePath)
                    re.mesh = Mesh::cache(filePath);
            }
        }
        else re.mesh = nullptr;

        if (false && j.find("Material") != j.end())
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
                        infos.push_back({ Shader::Vertex, jSha["Vertex"].get<std::string>() });

                    if (jSha.find("Geometry") != jSha.end())
                        infos.push_back({ Shader::Geometry, jSha["Geometry"].get<std::string>() });

                    if (jSha.find("TessControl") != jSha.end())
                        infos.push_back({ Shader::TessControl, jSha["TessControl"].get<std::string>() });

                    if (jSha.find("TessEvaluation") != jSha.end())
                        infos.push_back({ Shader::TessEvaluation, jSha["TessEvaluation"].get<std::string>() });

                    if (jSha.find("Pixel") != jSha.end())
                        infos.push_back({ Shader::Pixel, jSha["Pixel"].get<std::string>() });

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

    static void loadCollider(entt::entity entity, entt::registry& registry, const json& j)
    {
        
    }
    
    static void loadRigidBody(entt::entity entity, entt::registry& registry, const json& j)
    {
        
    }

    static void entityToJson(entt::entity entity, entt::registry& registry, json& j);
    
    static void entityFromJson(entt::entity entity, entt::registry& registry, json& j);

    void registryToSceneFile(entt::registry& registry, const std::string& sceneName)
    {
        
        json sceneJson;
        
        using component::EntityInfo;
        registry.each([&registry, &sceneJson](const entt::entity entity)
        {
            EntityInfo& info = registry.get<EntityInfo>(entity);
            entityToJson(entity, registry, sceneJson[info.name]);
        });
        //for (auto entity : registry)
        //{
        //    EntityInfo& info = registry.get<EntityInfo>(entity);
        //    entityToJson(entity, registry, sceneJson[info.name]);
        //}

        std::string sceneFilePath = "res/scenes/" + sceneName + ".oylscene";
        std::ofstream sceneFile(sceneFilePath);
        if (sceneFile)
        {
            sceneFile << std::setw(4) << sceneJson;
            sceneFile.flush();
        }
    }

    void registryFromSceneFile(entt::registry& registry, const std::string& sceneName)
    {
        std::string sceneFilePath = "res/scenes/" + sceneName + ".oylscene";
        std::ifstream sceneFile(sceneFilePath);
        if (!sceneFile)
        {
            OYL_LOG_ERROR("Failed to load scene \"{0}\"", sceneFilePath);
            return;
        }

        json sceneJson;
        sceneFile >> sceneJson;
        sceneFile.close();

        using component::EntityInfo;
        
        std::unordered_set<std::string> processedEntities;
        processedEntities.reserve(sceneJson.size());

        //for (auto entity : registry)
        registry.each([&registry, &sceneJson, &processedEntities](const entt::entity entity)
        {
            if (registry.has<EntityInfo>(entity))
            {
                EntityInfo& ei = registry.get<EntityInfo>(entity);
                if (sceneJson.find(ei.name) != sceneJson.end())
                {
                    entityFromJson(entity, registry, sceneJson[ei.name]);
                    processedEntities.emplace(ei.name);
                }
            }
        });
        
        for (auto& [key, value] : sceneJson.items())
        {
            if (processedEntities.find(key) != processedEntities.end())
                continue;

            auto entity = registry.create();

            EntityInfo info{ key };
            registry.assign<EntityInfo>(entity, std::move(info));

            entityFromJson(entity, registry, value);
        }
    }

    void entityToPrefabFile(entt::entity entity, entt::registry& registry, json& j) {}

    void entityfromPrefabFile(entt::entity entity, entt::registry& registry, json& j) {}

    static void entityToJson(entt::entity entity, entt::registry& registry, json& j)
    {
        saveTransform(entity, registry, j["Transform"]);
        saveParent(entity, registry, j["Parent"]);
        saveRenderable(entity, registry, j["Renderable"]);
    }

    static void entityFromJson(entt::entity entity, entt::registry& registry, json& j)
    {
        if (auto it = j.find("Transform"); it != j.end())
            loadTransform(entity, registry, it.value());

        if (auto it = j.find("Parent"); it != j.end())
            loadParent(entity, registry, it.value());

        if (auto it = j.find("Renderable"); it != j.end())
            loadRenderable(entity, registry, it.value());
    }
}
