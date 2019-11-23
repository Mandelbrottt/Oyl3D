#include "oylpch.h"
#include "SceneToFile.h"

#include "ECS/Component.h"

#include "Scenes/Scene.h"

namespace oyl::internal
{
    static void saveTransforms(json& j, entt::registry& registry);
    static void saveParents(json& j, entt::registry& registry);

    void saveSceneToFile(const Scene& scene)
    {
        json sceneJson;

        saveTransforms(sceneJson, *scene.m_registry);
        saveParents(sceneJson, *scene.m_registry);

        std::ofstream sceneFile("res/scenes/" + scene.m_name + ".oylscene");
        sceneFile << std::setw(4) << sceneJson;
    }

    static void loadTransform(const json& j, entt::registry& registry, entt::entity entity);
    static void loadParent(const json& j, entt::registry& registry, entt::entity entity);

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
            bool isInScene = false;

            for (auto entity : view)
            {
                SceneObject& so = view.get(entity);
                if (key == so.name)
                {
                    isInScene = true;

                    if (value.find("Transform") != value.end())
                        loadTransform(value["Transform"], *scene.m_registry, entity);

                    if (value.find("Parent") != value.end() &&
                        value["Parent"]["Name"].get<std::string>() != key)
                        loadParent(value["Parent"], *scene.m_registry, entity);

                    break;
                }

            }
            if (!isInScene)
            {
                auto entity = scene.m_registry->create();
                
                if (value.find("Transform") != value.end())
                    loadTransform(value["Transform"], *scene.m_registry, entity);

                if (value.find("Parent") != value.end())
                    loadParent(sceneJson, *scene.m_registry, entity);
            }
        }
    }

    void saveSceneBackupToFile(const Scene& scene)
    {
        json          sceneBackup;
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

            j[so.name]["Transform"]["Position"]["X"] = t.getPositionX();
            j[so.name]["Transform"]["Position"]["Y"] = t.getPositionY();
            j[so.name]["Transform"]["Position"]["Z"] = t.getPositionZ();

            j[so.name]["Transform"]["Rotation"]["X"] = t.getRotationEulerX();
            j[so.name]["Transform"]["Rotation"]["Y"] = t.getRotationEulerY();
            j[so.name]["Transform"]["Rotation"]["Z"] = t.getRotationEulerZ();

            j[so.name]["Transform"]["Scale"]["X"] = t.getScaleX();
            j[so.name]["Transform"]["Scale"]["Y"] = t.getScaleY();
            j[so.name]["Transform"]["Scale"]["Z"] = t.getScaleZ();
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
            auto& p = view.get<Parent>(entity);

            if (registry.valid(p.parent))
                j[so.name]["Parent"]["Name"] = registry.get<SceneObject>(p.parent).name;
        }
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
}
