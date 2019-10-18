#pragma once

#include "Oyl3D/Events/Event.h"

#include <entt/entt.hpp>

namespace oyl::ECS
{
    class Registry : public entt::registry
    {
    public:
        using entt::registry::registry;
    };
}