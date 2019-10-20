#pragma once
#include "Oyl3D/ECS/Registry.h"

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

namespace oyl
{
    class Camera;
}

namespace oyl::ECS
{
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

    class System : public ISystem, public EventListener
    {
    public:
        explicit System()  = default;
        virtual  ~System() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        void setRegistry(Ref<Registry> reg) { registry = std::move(reg); }

    protected:
        Ref<Registry> registry;
    };

    class RenderSystem : public System
    {
    public:
        explicit RenderSystem()  = default;
        virtual  ~RenderSystem() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;
    };

    class PhysicsSystem : public System
    {
    public:
        explicit PhysicsSystem()  = default;
        virtual  ~PhysicsSystem() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;
    };

    class OracleCameraSystem : public System
    {
    public:
        explicit OracleCameraSystem()  = default;
        virtual  ~OracleCameraSystem() = default;

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
