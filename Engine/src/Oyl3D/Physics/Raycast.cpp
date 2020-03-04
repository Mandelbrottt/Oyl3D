#include "oylpch.h"

#include "Raycast.h"

#include "Physics/PhysicsSystems.h"

namespace oyl
{
    namespace RayTest
    {
        Ref<ClosestRaycastResult> Closest(glm::vec3 from, glm::vec3 direction, f32 distance, u16 mask)
        {
            return internal::PhysicsSystem::raytestClosest(from, direction, distance, mask);
        }
    }
}