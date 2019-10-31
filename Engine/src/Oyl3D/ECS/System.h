#pragma once

//#include "Oyl3D/Events/Event.h"
#include "Oyl3D/ECS/Registry.h"

#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    class Camera;
}

namespace oyl::ECS
{
    class ISystem : public virtual Node
    {
    public:
        explicit ISystem()  = default;
        virtual  ~ISystem() = default;

        virtual void onEnter() override = 0;
        virtual void onExit() override = 0;

        virtual void onUpdate(Timestep dt) override = 0;
        virtual void onGuiRender(Timestep dt) override = 0;
    };

    class System : public ISystem, public EventListener
    {
    protected:
        explicit System() = default;

    public:
        virtual ~System() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        void setRegistry(Ref<Registry> reg);

    protected:
        Ref<Registry> registry;
    };

    // TODO: Make standalone layer
    class RenderSystem : public System
    {
        OYL_CTOR(RenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        virtual bool onEvent(Ref<Event> event) override;

        void init();
        void shutdown();
    };

    class PhysicsSystem : public System
    {
        OYL_CTOR(PhysicsSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep) override;

        virtual bool onEvent(Ref<Event> event) override;
    };
    
    namespace internal
    {
        class OracleCameraSystem : public System
        {
            OYL_CTOR(OracleCameraSystem, System)

            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate(Timestep dt) override;
            virtual void onGuiRender(Timestep dt) override;

            virtual bool onEvent(Ref<Event> event) override;

        private:
            void processCameraUpdate(Timestep dt, const Ref<Camera>& camera);

            glm::vec3 m_cameraMove        = glm::vec3(0.0f);
            float     m_cameraMoveSpeed   = 15.0f;
            glm::vec3 m_cameraRotate      = glm::vec3(0.0f);
            float     m_cameraRotateSpeed = 25.0f;

            bool m_doMoveCamera = false;
        };
    }
}
