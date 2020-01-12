#include "oylpch.h"

#include "Raycast.h"

#include "ECS/SystemImpl.h"

namespace oyl
{
    namespace RayTest
    {
        Ref<ClosestRaycastResult> Closest(glm::vec3 from, glm::vec3 direction, f32 distance)
        {
            return internal::PhysicsSystem::raytestClosest(from, direction, distance);
        }
    }
}