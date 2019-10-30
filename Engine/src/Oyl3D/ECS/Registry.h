#pragma once

#include <entt/entt.hpp>

namespace oyl::ECS
{
    class Registry : public entt::registry
    {
    public:
        using entt::registry::registry;
    };
}