#include "oylpch.h"

#include "Layer.h"

namespace oyl
{
    Layer::Layer(const std::string& debugName)
    #if defined(OYL_LOG_CONSOLE)
        : m_debugName(debugName)
    #endif
    {
    }

    Layer::~Layer()
    {
    }
}
