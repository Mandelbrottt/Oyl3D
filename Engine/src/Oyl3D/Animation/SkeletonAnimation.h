#pragma once

namespace oyl
{
    struct Bone
    {
        uint parent = -1u;
        std::vector<uint> children;

        glm::mat4 transform = glm::mat4(1.0f);
        glm::mat4 inverseTransform = glm::mat4(1.0f);
        mutable glm::mat4 finalTransform = glm::mat4(1.0f);
    };

    struct BoneChannel
    {
        std::vector<std::pair<float, glm::vec3>> positionKeys;
        std::vector<std::pair<float, glm::quat>> rotationKeys;
        std::vector<std::pair<float, glm::vec3>> scaleKeys;
    };
    
    struct SkeletonAnimation
    {
        std::unordered_map<uint, BoneChannel> channels;
        
        float tickRate = 30.0f;
        float duration = 0.0f;
    };
}