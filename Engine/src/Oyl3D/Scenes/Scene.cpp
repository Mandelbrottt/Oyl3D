#include "oylpch.h"
#include "Scene.h"

#include "ECS/SystemImpl.h"
#include "ECS/Registry.h"
#include "ECS/Component.h"

#include "Events/EventDispatcher.h"

namespace oyl
{
    WeakRef<Scene> Scene::s_current{};

    Scene::Scene()
        : m_registry(Ref<Registry>::create()),
          m_physicsSystem(internal::PhysicsSystem::create()),
          m_transformUpdateSystem(internal::TransformUpdateSystem::create())
    {
        m_physicsSystem->onEnter();
        m_transformUpdateSystem->onEnter();
    }

    Scene::~Scene()
    {
        saveSceneBackupToFile();
        
        // Reset the registry then reset the actual Ref
        m_registry->reset();
        m_registry.reset();
    }

    void Scene::onEnter() { }

    void Scene::onExit() { }

    void Scene::onUpdate(Timestep dt)
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onUpdateSystems(dt);
            layer->onUpdate(dt);
        }

        m_physicsSystem->onUpdate(dt);
        m_transformUpdateSystem->onUpdate(dt);
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

        m_physicsSystem->onGuiRender(dt);
        m_transformUpdateSystem->onGuiRender(dt);
    }

    void Scene::initDefaultSystems()
    {
        m_physicsSystem->setRegistry(m_registry);
        m_physicsSystem->setDispatcher(m_dispatcher);

        m_transformUpdateSystem->setRegistry(m_registry);
        m_transformUpdateSystem->setDispatcher(m_dispatcher);
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

    void Scene::loadSceneFromFile()
    {
        using component::SceneObject;
        using component::Transform;
        
        //m_registry->each([this](auto entity)
        //{
        //    m_registry->assign<SceneIntrinsic>(entity);
        //    if (!m_registry->has<SceneObject>(entity))
        //    {
        //        auto& so = m_registry->assign<SceneObject>(entity);
        //        
        //        char buf[128];
        //        sprintf(buf, "Entity %d", (u32)entity);
        //        so.name = std::string(buf);
        //    }
        //});

        std::ifstream sceneFile("res/scenes/" + m_name + ".oylscene");
        if (!sceneFile)
            return;
        
        json sceneJson;
        sceneFile >> sceneJson;
        sceneFile.close();

        auto view = m_registry->view<SceneObject>();
        for (auto& [key, value] : sceneJson.items())
        {
            for (auto entity : view)
            {
                SceneObject& so = view.get(entity);
                if (key == so.name)
                {
                    auto& t = m_registry->get_or_assign<Transform>(entity);
                    t.setPositionX(value["Transform"]["Position"]["X"].get<float>());
                    t.setPositionY(value["Transform"]["Position"]["Y"].get<float>());
                    t.setPositionZ(value["Transform"]["Position"]["Z"].get<float>());

                    t.setRotationEulerX(value["Transform"]["Rotation"]["X"].get<float>());
                    t.setRotationEulerY(value["Transform"]["Rotation"]["Y"].get<float>());
                    t.setRotationEulerZ(value["Transform"]["Rotation"]["Z"].get<float>());

                    t.setScaleX(value["Transform"]["Scale"]["X"].get<float>());
                    t.setScaleY(value["Transform"]["Scale"]["Y"].get<float>());
                    t.setScaleZ(value["Transform"]["Scale"]["Z"].get<float>());

                    // Other components...

                    break;
                }
            }
        }

        //auto view = m_registry->view<SceneObject, Transform>();
        //for (auto entity : view)
        //{
        //    auto& so = view.get<SceneObject>(entity);
        //    auto& t = view.get<Transform>(entity);
        //    sceneJson[so.name]["Transform"]["Position"]["X"] = t.position.x;
        //    sceneJson[so.name]["Transform"]["Position"]["Y"] = t.position.y;
        //    sceneJson[so.name]["Transform"]["Position"]["Z"] = t.position.z;

        //    sceneJson[so.name]["Transform"]["Rotation"]["X"] = t.rotation.x;
        //    sceneJson[so.name]["Transform"]["Rotation"]["Y"] = t.rotation.y;
        //    sceneJson[so.name]["Transform"]["Rotation"]["Z"] = t.rotation.z;

        //    sceneJson[so.name]["Transform"]["Scale"]["X"] = t.scale.x;
        //    sceneJson[so.name]["Transform"]["Scale"]["Y"] = t.scale.y;
        //    sceneJson[so.name]["Transform"]["Scale"]["Z"] = t.scale.z;
        //}

        //m_registry->each(
        //    [&](auto entity)
        //    {
        //        m_registry->assign<SceneIntrinsic>(entity);
        //    });
    }

    void Scene::saveSceneToFile()
    {
        json sceneJson;

        using component::SceneObject;
        using component::Transform;
        auto view = m_registry->view<SceneObject, Transform>();
        for (auto entity : view)
        {
            auto& so = view.get<SceneObject>(entity);
            auto& t =  view.get<Transform>(entity);
            sceneJson[so.name]["Transform"]["Position"]["X"] = t.getPositionX();
            sceneJson[so.name]["Transform"]["Position"]["Y"] = t.getPositionY();
            sceneJson[so.name]["Transform"]["Position"]["Z"] = t.getPositionZ();

            sceneJson[so.name]["Transform"]["Rotation"]["X"] = t.getRotationEulerX();
            sceneJson[so.name]["Transform"]["Rotation"]["Y"] = t.getRotationEulerY();
            sceneJson[so.name]["Transform"]["Rotation"]["Z"] = t.getRotationEulerZ();

            sceneJson[so.name]["Transform"]["Scale"]["X"] = t.getScaleX();
            sceneJson[so.name]["Transform"]["Scale"]["Y"] = t.getScaleY();
            sceneJson[so.name]["Transform"]["Scale"]["Z"] = t.getScaleZ();
        }
        
        std::ofstream sceneFile("res/scenes/" + m_name + ".oylscene");
        sceneFile << std::setw(4) << sceneJson;
    }

    void Scene::saveSceneBackupToFile()
    {
        json sceneBackup;
        std::ifstream sceneFile("res/scenes/" + m_name + ".oylscene");
        if (sceneFile)
        {
            sceneFile >> sceneBackup;
            
            std::ofstream sceneBackupFile("res/scenes/" + m_name + ".oylscene.backup");
            sceneBackupFile << std::setw(4) << sceneBackup;
        }
    }
}
