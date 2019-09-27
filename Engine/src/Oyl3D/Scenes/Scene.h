#pragma once

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/LayerStack.h"

namespace oyl
{
    class Scene : public EventListener
    {
        friend class Application;
    public:
        explicit Scene(const std::string& debugName = "Scene");
        virtual ~Scene();

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(Ref<Layer> layer);
        void popOverlay(Ref<Layer> overlay);
        
        virtual void setPostEventCallback(PostEventFn callback) override;
        virtual void setRegisterCallback(RegisterFn callback) override;
        virtual void setUnregisterCallback(UnregisterFn callback) override;
        
        virtual bool onEvent(Ref<Event> event) override;
    private:
        void onUpdate(Timestep dt);
        void onGuiRender();
        
    private:
        LayerStack m_layerStack;

#if defined(OYL_LOG_CONSOLE)
    protected:
        const std::string m_debugName;
#endif
    };
}
