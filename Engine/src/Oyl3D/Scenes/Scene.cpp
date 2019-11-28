#include "oylpch.h"
#include "Scene.h"

#include "SystemsLayer.h"

#include "ECS/SystemImpl.h"
#include "ECS/Component.h"

#include "Events/EventDispatcher.h"

#include "Utils/SceneToFile.h"

namespace oyl
{
    WeakRef<Scene> Scene::s_current = {};
    
    Scene::Scene()
        : m_registry(Ref<entt::registry>::create()) {}

    Scene::~Scene() {}

    void Scene::onEnter() {}

    void Scene::onExit()
    {
        internal::saveSceneBackupToFile(*this);

        // Reset the registry then reset the actual Ref
        m_registry->reset();
        m_registry.reset();

    }

    void Scene::onUpdate(Timestep dt)
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onUpdateSystems(dt);
            layer->onUpdate(dt);
        }
    }

    bool Scene::onEvent(Ref<Event> event)
    {
        return false;
    }

    void Scene::onGuiRender(Timestep dt)
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onGuiRenderSystems(dt);
            layer->onGuiRender(dt);
        }
    }

    void Scene::pushLayer(Ref<Layer> layer)
    {
        layer->setRegistry(m_registry);
        layer->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(layer);
        
        m_layerStack.pushLayer(std::move(layer));
    }

    void Scene::pushOverlay(Ref<Layer> overlay)
    {
        overlay->setRegistry(m_registry);
        overlay->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(overlay);
        
        m_layerStack.pushOverlay(std::move(overlay));
    }

    void Scene::popLayer(const Ref<Layer>& layer)
    {
        layer->setRegistry(nullptr);
        layer->setDispatcher(nullptr);
        m_dispatcher->unregisterListener(layer);
        
        m_layerStack.popLayer(layer);
    }

    void Scene::popOverlay(const Ref<Layer>& overlay)
    {
        overlay->setRegistry(nullptr);
        overlay->setDispatcher(nullptr);
        m_dispatcher->unregisterListener(overlay);
        
        m_layerStack.popOverlay(overlay);
    }

    // TODO: Move into helper functions
    //void Scene::loadSceneFromFile()
    //{
    //    using component::SceneObject;
    //    using component::Transform;
    //    using component::Parent;

    //    std::ifstream sceneFile("res/scenes/" + m_name + ".oylscene");
    //    if (!sceneFile)
    //        return;
    //    
    //    json sceneJson;
    //    sceneFile >> sceneJson;
    //    sceneFile.close();

    //    auto view = m_registry->view<SceneObject>();
    //    for (auto& [key, value] : sceneJson.items())
    //    {
    //        for (auto entity : view)
    //        {
    //            SceneObject& so = view.get(entity);
    //            if (key == so.name)
    //            {
    //                auto& t = m_registry->get_or_assign<Transform>(entity);
    //                t.setPositionX(value["Transform"]["Position"]["X"].get<float>());
    //                t.setPositionY(value["Transform"]["Position"]["Y"].get<float>());
    //                t.setPositionZ(value["Transform"]["Position"]["Z"].get<float>());

    //                t.setRotationEulerX(value["Transform"]["Rotation"]["X"].get<float>());
    //                t.setRotationEulerY(value["Transform"]["Rotation"]["Y"].get<float>());
    //                t.setRotationEulerZ(value["Transform"]["Rotation"]["Z"].get<float>());

    //                t.setScaleX(value["Transform"]["Scale"]["X"].get<float>());
    //                t.setScaleY(value["Transform"]["Scale"]["Y"].get<float>());
    //                t.setScaleZ(value["Transform"]["Scale"]["Z"].get<float>());

    //                if (value.find("Parent") != value.end() &&
    //                    value["Parent"].find("Name") != value["Parent"].end() &&
    //                    value["Parent"]["Name"].is_string())
    //                {
    //                    std::string name = value["Parent"]["Name"].get<std::string>();
    //                    if (name != so.name)
    //                        for (auto e : view)
    //                        {
    //                            if (view.get(e).name == name)
    //                            {
    //                                auto& p = m_registry->get_or_assign<Parent>(entity);
    //                                p.parent = e;
    //                            }
    //                        }
    //                }

    //                break;
    //            }
    //        }
    //    }
    //}

    //void Scene::saveSceneToFile()
    //{
    //    json sceneJson;

    //    using component::SceneObject;
    //    using component::Transform;
    //    using component::Parent;

    //    {
    //        auto view = m_registry->view<SceneObject, Transform>();
    //        for (auto entity : view)
    //        {
    //            auto& so = view.get<SceneObject>(entity);
    //            auto& t  = view.get<Transform>(entity);
    //            
    //            sceneJson[so.name]["Transform"]["Position"]["X"] = t.getPositionX();
    //            sceneJson[so.name]["Transform"]["Position"]["Y"] = t.getPositionY();
    //            sceneJson[so.name]["Transform"]["Position"]["Z"] = t.getPositionZ();

    //            sceneJson[so.name]["Transform"]["Rotation"]["X"] = t.getRotationEulerX();
    //            sceneJson[so.name]["Transform"]["Rotation"]["Y"] = t.getRotationEulerY();
    //            sceneJson[so.name]["Transform"]["Rotation"]["Z"] = t.getRotationEulerZ();

    //            sceneJson[so.name]["Transform"]["Scale"]["X"] = t.getScaleX();
    //            sceneJson[so.name]["Transform"]["Scale"]["Y"] = t.getScaleY();
    //            sceneJson[so.name]["Transform"]["Scale"]["Z"] = t.getScaleZ();
    //        }
    //    }
    //    {
    //        auto view = m_registry->view<SceneObject, Parent>();
    //        for (auto entity : view)
    //        {
    //            auto& so = view.get<SceneObject>(entity);
    //            auto& p  = view.get<Parent>(entity);

    //            if (m_registry->valid(p.parent))
    //                sceneJson[so.name]["Parent"]["Name"] = m_registry->get<SceneObject>(p.parent).name;
    //        }
    //    }
    //    
    //    std::ofstream sceneFile("res/scenes/" + m_name + ".oylscene");
    //    sceneFile << std::setw(4) << sceneJson;
    //}

    //void Scene::saveSceneBackupToFile()
    //{
    //    json sceneBackup;
    //    std::ifstream sceneFile("res/scenes/" + m_name + ".oylscene");
    //    if (sceneFile)
    //    {
    //        sceneFile >> sceneBackup;
    //        
    //        std::ofstream sceneBackupFile("res/scenes/" + m_name + ".oylscene.backup");
    //        sceneBackupFile << std::setw(4) << sceneBackup;
    //    }
    //}
}
