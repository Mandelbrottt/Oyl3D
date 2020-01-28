#include "oylpch.h"
#include "SceneToFile.h"

#include "Components/Collidable.h"
#include "Components/Lights.h"
#include "Components/Misc.h"
#include "Components/Renderable.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Scenes/Scene.h"

namespace glm
{
    void to_json(json& j, const vec3& v)
    {
        j = json({ v.x, v.y, v.z });
    }

    void from_json(const json& j, vec3& v)
    {
        int i = 0;
        for (const auto& it : j)
            it.get_to(v[i++]);
    }

    void to_json(json& j, const vec2& v)
    {
        j = json({ v.x, v.y });
    }

    void from_json(const json& j, vec2& v)
    {
        int i = 0;
        for (const auto& it : j)
            it.get_to(v[i++]);
    }
}

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
        
        auto& re = registry.get<Renderable>(entity);

        j["Enabled"] = re.enabled;
        
        auto& jMesh = j["Mesh"];
        if (re.mesh)
        {
            jMesh["FilePath"] = re.mesh->getFilePath();
            const auto& alias = Mesh::getAlias(re.mesh);
            auto& jAlias = jMesh["Alias"];
            if (alias != INVALID_ALIAS)
                jAlias = alias;
        }

        json& jMat = j["Material"];
        json& jAlias = jMat["Alias"];
        if (re.material)
        {
            if (Material::isCached(re.material))
            {
                std::string alias = Material::getAlias(re.material);
                jAlias = alias;
            }
        }
    }

    static void saveGuiRenderable(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::EntityInfo;
        using component::GuiRenderable;

        auto& re = registry.get<GuiRenderable>(entity);

        j["Enabled"] = re.enabled;

        auto& jTexture = j["Texture"];
        if (re.texture)
        {
            jTexture["FilePath"] = re.texture->getFilePath();
            const auto& alias = Texture2D::getAlias(re.texture);
            auto& jAlias = jTexture["Alias"];
            if (alias != INVALID_ALIAS)
                jAlias = alias;
        }

        auto& jClip = j["Clipping"];
        jClip["Lower"] = re.lowerClipping;
        jClip["Upper"] = re.upperClipping;
    }

    static void saveCollidable(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::EntityInfo;
        using component::Collidable;

        auto& co = registry.get<Collidable>(entity);

        auto& jSha = j["Shapes"];
        jSha = json::array();

        for (auto& info : co)
        {
            json shape = json::object();

            shape["Trigger"] = info.isTrigger();
            shape["Type"] = static_cast<uint>(info.getType());

            switch (info.getType())
            {
                case ColliderType::Box:
                    shape["Center"] = info.box.getCenter();
                    shape["Size"]   = info.box.getSize();
                    break;
                case ColliderType::Sphere:
                    shape["Center"] = info.sphere.getCenter();
                    shape["Radius"] = info.sphere.getRadius();
                    break;
                case ColliderType::Capsule:
                    shape["Center"]    = info.capsule.getCenter();
                    shape["Direction"] = static_cast<uint>(info.capsule.getDirection());
                    shape["Height"]    = info.capsule.getHeight();
                    shape["Radius"]    = info.capsule.getRadius();
                    break;
                case ColliderType::Cylinder:
                    shape["Center"]    = info.cylinder.getCenter();
                    shape["Direction"] = static_cast<uint>(info.cylinder.getDirection());
                    shape["Height"]    = info.cylinder.getHeight();
                    shape["Radius"]    = info.cylinder.getRadius();
                    break;
                //case ColliderType::Mesh:
                //    break;
            }

            jSha.push_back(std::move(shape));
        }
    }
    
    static void saveRigidBody(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::RigidBody;

        auto& rb = registry.get<RigidBody>(entity);

        j["Mass"] = rb.getMass();
        j["Friction"] = rb.getFriction();
        j["Properties"] = rb.getPropertyFlags();
    }

    static void saveLightSource(entt::entity entity, entt::registry& registry, json& j)
    {
        using component::PointLight;

        auto& ls = registry.get<PointLight>(entity);

        j = json::array();

        json jLight = json::object();
        
        jLight["Ambient"] = ls.ambient;
        jLight["Diffuse"] = ls.diffuse;
        jLight["Specular"] = ls.specular;
        jLight["Attenuation"] = ls.attenuation;

        j.push_back(std::move(jLight));
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
            bool doLoad = false;
            if (auto alIt = it->find("Alias"); alIt != it->end() && alIt->is_string())
            {
                if (Mesh::exists(alIt->get<std::string>()))
                    re.mesh = Mesh::get(alIt->get<std::string>());
                else
                    doLoad = true;
            } else doLoad = true;
            
            if (auto fpIt = it->find("FilePath"); doLoad && fpIt != it->end() && !fpIt->is_null())
            {
                auto filePath = fpIt->get<std::string>();

                bool meshFound = false;
                for (const auto& [alias, mesh] : Mesh::getCache())
                {
                    if (mesh->getFilePath() == filePath)
                        meshFound = true, re.mesh = mesh;
                }

                if (!meshFound && (!re.mesh || re.mesh && re.mesh->getFilePath() != filePath))
                    re.mesh = Mesh::cache(filePath);
            }
        }
        else re.mesh = nullptr;

        if (j.find("Material") != j.end())
        {
            auto& jMat = j["Material"];

            if (auto it = jMat.find("Alias"); it != jMat.end() && it->is_string())
            {
                std::string alias = it->get<std::string>();
                if (Material::exists(alias))
                    re.material = Material::get(alias);
                else
                    re.material = materialFromFile("res/assets/materials/" + alias + ".oylmat");
            }
        }
    }

    static void loadGuiRenderable(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::GuiRenderable;

        auto& re = registry.get_or_assign<GuiRenderable>(entity);

        if (auto it = j.find("Enabled"); it != j.end())
            it->get_to(re.enabled);

        if (auto it = j.find("Texture"); it != j.end() && !it->is_null())
        {
            bool doLoad = false;
            if (auto alIt = it->find("Alias"); alIt != it->end() && alIt->is_string())
            {
                if (Texture2D::exists(alIt->get<std::string>()))
                    re.texture = Texture2D::get(alIt->get<std::string>());
                else
                    doLoad = true;
            } else doLoad = true;
            
            if (auto fpIt = it->find("FilePath"); doLoad && fpIt != it->end() && !fpIt->is_null())
            {
                auto filePath = fpIt->get<std::string>();

                bool meshFound = false;
                for (const auto& [alias, texture] : Texture2D::getCache())
                {
                    if (texture->getFilePath() == filePath)
                        meshFound = true, re.texture = texture;
                }

                if (!meshFound && (!re.texture || re.texture && re.texture->getFilePath() != filePath))
                    re.texture = Texture2D::cache(filePath);
            }
        } else re.texture = nullptr;

        if (auto it = j.find("Clipping"); it != j.end() && it->is_object())
        {
            if (auto clIt = it->find("Lower"); clIt != it->end() && it->is_array())
                clIt->get_to(re.lowerClipping);
            if (auto clIt = it->find("Upper"); clIt != it->end() && it->is_array())
                clIt->get_to(re.upperClipping);
        }
    }

    static void loadCollidable(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::Collidable;

        auto& co = registry.assign_or_replace<Collidable>(entity);

        if (const auto sIt = j.find("Shapes"); sIt != j.end() && sIt->is_array())
        {
            for (const auto& jShape : sIt.value())
            {
                ColliderType type = static_cast<ColliderType>(jShape.at("Type").get<uint>());
                auto& shape = co.pushShape(type);
                shape.setIsTrigger(jShape.at("Trigger").get<bool>());

                switch (type)
                {
                    case ColliderType::Box:
                        if (auto it = jShape.find("Center"); it != jShape.end())
                            shape.box.setCenter(it->get<glm::vec3>());
                        if (auto it = jShape.find("Size"); it != jShape.end())
                            shape.box.setSize(it->get<glm::vec3>());
                        break;
                    case ColliderType::Sphere:
                        if (auto it = jShape.find("Center"); it != jShape.end())
                            shape.sphere.setCenter(it->get<glm::vec3>());
                        if (auto it = jShape.find("Radius"); it != jShape.end())
                            shape.sphere.setRadius(it->get<float>());
                        break;
                    case ColliderType::Capsule:
                        if (auto it = jShape.find("Center"); it != jShape.end())
                            shape.capsule.setCenter(it->get<glm::vec3>());
                        if (auto it = jShape.find("Direction"); it != jShape.end())
                            shape.capsule.setDirection(static_cast<Direction>(it->get<uint>()));
                        if (auto it = jShape.find("Height"); it != jShape.end())
                            shape.capsule.setHeight(it->get<float>());
                        if (auto it = jShape.find("Radius"); it != jShape.end())
                            shape.capsule.setRadius(it->get<float>());
                        break;
                    case ColliderType::Cylinder:
                        if (auto it = jShape.find("Center"); it != jShape.end())
                            shape.cylinder.setCenter(it->get<glm::vec3>());
                        if (auto it = jShape.find("Direction"); it != jShape.end())
                            shape.cylinder.setDirection(static_cast<Direction>(it->get<uint>()));
                        if (auto it = jShape.find("Height"); it != jShape.end())
                            shape.cylinder.setHeight(it->get<float>());
                        if (auto it = jShape.find("Radius"); it != jShape.end())
                            shape.cylinder.setRadius(it->get<float>());
                        break;
                    //case ColliderType::Mesh:
                    //    break;
                }
            }
        }
    }
    
    static void loadRigidBody(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::RigidBody;

        auto& rb = registry.get_or_assign<RigidBody>(entity);

        if (auto it = j.find("Friction"); it != j.end() && it->is_number_float())
            rb.setFriction(it->get<float>());

        if (auto it = j.find("Mass"); it != j.end() && it->is_number_float())
            rb.setMass(it->get<float>());

        if (auto it = j.find("Properties"); it != j.end() && it->is_number_unsigned())
            rb.setPropertyFlags(it->get<uint>());
    }

    static void loadLightSource(entt::entity entity, entt::registry& registry, const json& j)
    {
        using component::PointLight;

        auto& ls = registry.assign_or_replace<PointLight>(entity);

        if (!j.is_array()) 
            return;
        
        for (const auto& jLight : j)
        {
            if (const auto it = jLight.find("Ambient"); it != jLight.end())
                it->get_to(ls.ambient);
            if (const auto it = jLight.find("Diffuse"); it != jLight.end())
                it->get_to(ls.diffuse);
            if (const auto it = jLight.find("Specular"); it != jLight.end())
                it->get_to(ls.specular);
            if (const auto it = jLight.find("Attenuation"); it != jLight.end())
                it->get_to(ls.attenuation);
        }
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

        auto view = registry.view<EntityInfo>(); 
        for (auto& [key, value] : sceneJson.items())
        {
            bool exists = false;
            for (auto entity : view)
                exists |= view.get(entity).name == key;

            if (!exists)
            {
                auto entity = registry.create();
                EntityInfo info = { key };
                registry.assign<EntityInfo>(entity, info);
            }
        }

        view = registry.view<EntityInfo>();
        for (auto entity : view)
        {
            EntityInfo& info = view.get(entity);
            if (sceneJson.find(info.name) != sceneJson.end())
            {
                entityFromJson(entity, registry, sceneJson[info.name]);
                processedEntities.emplace(info.name);
            }
        }
        
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

        if (registry.has<component::Renderable>(entity))
            saveRenderable(entity, registry, j["Renderable"]);

        if (registry.has<component::GuiRenderable>(entity))
            saveGuiRenderable(entity, registry, j["GuiRenderable"]);

        if (registry.has<component::Collidable>(entity))
            saveCollidable(entity, registry, j["Collidable"]);

        if (registry.has<component::RigidBody>(entity))
            saveRigidBody(entity, registry, j["RigidBody"]);

        if (registry.has<component::PointLight>(entity))
            saveLightSource(entity, registry, j["LightSource"]);
    }

    static void entityFromJson(entt::entity entity, entt::registry& registry, json& j)
    {
        if (auto it = j.find("Transform"); it != j.end())
            loadTransform(entity, registry, it.value());

        if (auto it = j.find("Parent"); it != j.end())
            loadParent(entity, registry, it.value());

        if (auto it = j.find("Renderable"); it != j.end())
            loadRenderable(entity, registry, it.value());

        if (auto it = j.find("GuiRenderable"); it != j.end())
            loadGuiRenderable(entity, registry, it.value());

        if (auto it = j.find("Collidable"); it != j.end())
            loadCollidable(entity, registry, it.value());

        if (auto it = j.find("RigidBody"); it != j.end())
            loadRigidBody(entity, registry, it.value());

        if (auto it = j.find("LightSource"); it != j.end())
            loadLightSource(entity, registry, it.value());
    }

    Ref<Material> materialFromFile(const std::string& filepath)
    {
        std::ifstream materialFile(filepath);
        if (!materialFile)
        {
            OYL_LOG_ERROR("Failed to open material file \"{0}\"!", filepath);
            return nullptr;
        }

        json jFile;
        materialFile >> jFile;
        materialFile.close();

        const json& jMaterial = jFile["Material"];

        Ref<Material> material = Material::create();

        if (auto it = jMaterial.find("Shader"); it != jMaterial.end() && !it->is_null())
        {
            bool loadNewShader = false;
            std::string alias;
            if (auto alIt = it->find("Alias"); alIt != it->end() && alIt->is_string())
            {
                alIt->get_to(alias);
                if (Shader::exists(alias))
                    material->shader = Shader::get(alias);
                else
                    loadNewShader = true;
            }
            else loadNewShader = true;

            if (loadNewShader)
            {
                std::vector<ShaderInfo> infos;
                infos.reserve(5);

                auto pushBackFn = [&infos, &it](const char* key, Shader::Type type)
                {
                    if (auto sIt = it->find(key);
                        sIt != it->end() &&
                        sIt->contains("guid") &&
                        sIt->contains("FilePath"))
                    {
                        ShaderInfo info;
                        info.type = type;
                        info.filename = sIt->at("FilePath").get<std::string>();

                        infos.push_back(std::move(info));
                    }
                };

                pushBackFn("Vertex",         Shader::Type::Vertex);
                pushBackFn("Geometry",       Shader::Type::Geometry);
                pushBackFn("TessControl",    Shader::Type::TessControl);
                pushBackFn("TessEvaluation", Shader::Type::TessEvaluation);
                pushBackFn("Pixel",          Shader::Type::Fragment);

                if (!alias.empty())
                    material->shader = Shader::cache(infos, alias, true);
                else
                    material->shader = Shader::create(infos);
            }
        }

        if (auto it = jMaterial.find("Textures"); it != jMaterial.end())
        {
            auto getTextureFn = [](json::const_iterator& it)
            {
                std::string tFilepath = {};
                if (auto filepathIt = it->find("FilePath");
                    filepathIt != it->end() && filepathIt->is_string())
                    filepathIt->get_to(tFilepath);

                if (auto aliasIt = it->find("Alias"); aliasIt != it->end() && aliasIt->is_string())
                {
                    std::string alias = aliasIt->get<std::string>();
                    if (Texture2D::exists(alias))
                        return Texture2D::get(alias);
                    else
                    {
                        // HACK: Incorporate properties into cache function somehow
                        auto& tex = Texture2D::cache(tFilepath, alias);
                        if (auto prIt = it->find("Profile"); prIt != it->end() && prIt->is_number_unsigned())
                            tex->setProfile(static_cast<TextureProfile>(prIt->get<uint>()));
                        return tex;
                    }
                }
                else if (!tFilepath.empty())
                    return Texture2D::create(tFilepath);
                else
                    return Ref<Texture2D>(nullptr);
            };
            
            if (auto albedoIt = it->find("Albedo"); albedoIt != it->end())
                material->albedoMap = getTextureFn(albedoIt);

            if (auto specularIt = it->find("Specular"); specularIt != it->end())
                material->specularMap = getTextureFn(specularIt);
            
            if (auto normalIt = it->find("Normal"); normalIt != it->end())
                material->normalMap = getTextureFn(normalIt);
        }

        if (auto it = jMaterial.find("Alias"); it != jMaterial.end() && it->is_string())
        {
            std::string alias = it->get<std::string>();
            if (Material::exists(alias))
            {
                auto testMat = Material::get(alias);
                if (*material == *testMat)
                {
                    material = testMat;
                }
            }
        }

        if (auto it = jMaterial.find("MainProperties"); it != jMaterial.end() && it->is_object())
        {
            if (auto tileIt = it->find("Tiling"); tileIt != it->end() && tileIt->is_array())
                tileIt->get_to(material->mainTextureProps.tiling);
            if (auto offIt = it->find("Offset"); offIt != it->end() && offIt->is_array())
                offIt->get_to(material->mainTextureProps.offset);
        }
        
        return material;
    }

    void materialToFile(const Ref<Material>& material, const std::string& filepath)
    {
        if (!material)
            return;

        // TEMPORARY: Uncomment when no more changes to material file formatting are to be made
        //if (auto tempMat = materialFromFile(filepath); 
        //    tempMat != nullptr && *material == *tempMat)
        //    return;
        
        json jFile;
        json& jMaterial = jFile["Material"];

        json& jMatAlias = jMaterial["Alias"];
        if (auto alias = Material::getAlias(material); alias != INVALID_ALIAS)
            jMatAlias = alias;
        
        json& jShader = jMaterial["Shader"];
        if (material->shader)
        {
            json& jAlias = jShader["Alias"];
            if (auto alias = Shader::getAlias(material->shader); alias != INVALID_ALIAS)
                jAlias = alias;
            
            const auto& infos = material->shader->getShaderInfos();
            for (const auto& info : infos)
            {
                switch (info.type)
                {
                    case Shader::Type::Vertex:
                    {
                        json& jVert = jShader["Vertex"];
                        jVert["guid"];
                        jVert["FilePath"] = info.filename;
                        break;
                    }
                    case Shader::Type::Geometry:
                    {
                        json& jGeom = jShader["Geometry"];
                        jGeom["guid"];
                        jGeom["FilePath"] = info.filename;
                        break;
                    }
                    case Shader::Type::TessControl:
                    {
                        json& jTesc = jShader["TessControl"];
                        jTesc["guid"];
                        jTesc["FilePath"] = info.filename;
                        break;
                    }
                    case Shader::Type::TessEvaluation:
                    {
                        json& jTese = jShader["TessEvaluation"];
                        jTese["guid"];
                        jTese["FilePath"] = info.filename;
                        break;
                    }
                    case Shader::Type::Fragment:
                    {
                        json& jFrag = jShader["Pixel"];
                        jFrag["guid"];
                        jFrag["FilePath"] = info.filename;
                        break;
                    }
                }
            }
        }

        json& jTextures = jMaterial["Textures"];

        auto _texToJson = [](const Ref<Texture2D>& texture, json& jTex)
        {
            if (!texture) return;
            
            jTex["guid"];

            // TEMPORARY:
            jTex["FilePath"] = texture->getFilePath();

            json& jAlias = jTex["Alias"];
            if (auto alias = Texture2D::getAlias(texture); alias != INVALID_ALIAS)
                jAlias = alias;

            jTex["Profile"] = static_cast<uint>(texture->getProfile());
        };

        _texToJson(material->albedoMap, jTextures["Albedo"]);
        _texToJson(material->specularMap, jTextures["Specular"]);
        _texToJson(material->normalMap, jTextures["Normal"]);

        auto& jMainProps = jMaterial["MainProperties"];
        {
            jMainProps["Tiling"] = material->mainTextureProps.tiling;
            jMainProps["Offset"] = material->mainTextureProps.offset;
        }

        std::ofstream materialFile(filepath);
        if (materialFile)
        {
            materialFile << std::setw(4) << jFile;
            materialFile.flush();
        }
        else
        {
            OYL_LOG_ERROR("Failed to save material file \"{0}\"!", filepath);
        }
    }
}
