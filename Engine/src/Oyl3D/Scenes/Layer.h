#pragma once

#include "Oyl3D/Events/Event.h"

namespace oyl
{
    class Layer
    {
    public:
        Layer(const std::string& debugName = "Layer");
        virtual ~Layer();

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;

        virtual void onUpdate(Timestep dt)
        {
        }

        virtual void onImGuiRender()
        {
        }

        virtual void onEvent(Event& e)
        {
        }

#if defined(OYL_LOG_CONSOLE)
    protected:
        const std::string m_debugName;
#endif
    };
}
