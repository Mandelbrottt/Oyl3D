#pragma once

#include "Oyl3D/Debug/ImGuiLayer.h"
#include "Oyl3D/Events/Event.h"

#include "Oyl3D/Scenes/LayerStack.h"

namespace oyl
{
    class Scene
    {
        friend class Application;
    public:
        Scene(const std::string& debugName = "Scene");
        virtual ~Scene();

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(Ref<Layer> layer);
        void popOverlay(Ref<Layer> overlay);
    private:
        void onUpdate(Timestep dt);
        void onEvent(Event& e);
        void onImGuiRender();
    private:
        LayerStack m_layerStack;

#if defined(OYL_LOG_CONSOLE)
    protected:
        const std::string m_debugName;
#endif
    };
}
