#pragma once

namespace oyl
{
    struct ClosestRaycastResult;
    
    namespace RayTest
    {
        extern Ref<ClosestRaycastResult> Closest(glm::vec3 from, glm::vec3 direction, f32 distance);
    }
    
    struct RaycastResult
    {
        struct HitObject
        {
            entt::entity entity      = entt::null;
            glm::vec3    hitPosition = {};
            glm::vec3    hitNormal   = {};
            f32          hitFraction = {};
            f32          distanceTo  = {};
        };

        const glm::vec3 fromPosition = {};
        const glm::vec3 toPosition   = {};

        const bool hasHit = false;

    protected:
        RaycastResult(glm::vec3 from, glm::vec3 to, bool hit)
            : fromPosition(from), toPosition(to), hasHit(hit) {}
        
        virtual ~RaycastResult() = default;
    };

    struct ClosestRaycastResult : RaycastResult
    {
        explicit ClosestRaycastResult(glm::vec3 from, 
                                      glm::vec3 to,
                                      bool hit,
                                      HitObject obj)
            : RaycastResult(from, to, hit), hitObject(obj) {}
        
        virtual ~ClosestRaycastResult() = default;

        const HitObject hitObject;
    };

    struct AllHitsRaycastResult : RaycastResult
    {
        explicit AllHitsRaycastResult(glm::vec3 from, 
                                      glm::vec3 to, 
                                      bool hit,
                                      std::vector<HitObject>&& objs)
            : RaycastResult(from, to, hit), hitObjects(std::move(objs)) {}
        
        virtual ~AllHitsRaycastResult() = default;

        const std::vector<HitObject> hitObjects;
    };
}