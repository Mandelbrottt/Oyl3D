#pragma once

//#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    class Camera;
}

namespace oyl::ECS
{
    class Registry;

    class ISystem
    {
    public:
        explicit ISystem()  = default;
        virtual  ~ISystem() = default;

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        virtual void onUpdate(Timestep dt) = 0;
        virtual void onGuiRender() = 0;
    };

    class System : public ISystem, public EventListener, public Node
    {
    protected:
        explicit System(std::string name = "System");

    public:
        virtual ~System() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        void setRegistry(Ref<Registry> reg);

    protected:
        Ref<Registry> registry;
    };

    class RenderSystem : public System
    {
        struct _RenderSystem_
        {
        };

    public:
        explicit RenderSystem(_RenderSystem_)
            : System("RenderSystem")
        {
        }

        static Ref<RenderSystem> create();

        virtual ~RenderSystem() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;
    };

    class PhysicsSystem : public System
    {
        struct _PhysicsSystem_
        {
        };

    public:
        explicit PhysicsSystem(_PhysicsSystem_)
            : System("PhysicsSystem")
        {
        }

        virtual ~PhysicsSystem() = default;
        
        //static Ref<PhysicsSystem> create();

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;
    };

    class OracleCameraSystem : public System
    {
        struct _OracleCameraSystem_
        {
        };

    public:
        explicit OracleCameraSystem(_OracleCameraSystem_)
            : System("PhysicsSystem")
        {
        }

        virtual ~OracleCameraSystem() = default;

        static Ref<OracleCameraSystem> create();

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;

    private:
        void processCameraUpdate(Timestep dt, const Ref<Camera>& camera);

        glm::vec3 m_cameraMove        = glm::vec3(0.0f);
        float     m_cameraMoveSpeed   = 5.0f;
        glm::vec3 m_cameraRotate      = glm::vec3(0.0f);
        float     m_cameraRotateSpeed = 25.0f;

        bool m_doMoveCamera = false;
    };
}
