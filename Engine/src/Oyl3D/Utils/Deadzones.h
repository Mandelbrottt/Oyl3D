#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

static glm::vec2 radialDeadZone(glm::vec2 rawInput, float deadZone)
{
    if (glm::length2(rawInput) < deadZone * deadZone)
        return glm::vec2(0.0f);
    else
        return glm::normalize(rawInput) * (glm::length(rawInput) - deadZone) / (1 - deadZone);
}

static glm::vec2 crossDeadZone(glm::vec2 rawInput, float deadZone)
{
    glm::vec2 newInput = rawInput;

    if (abs(rawInput.x) < deadZone)
        newInput.x = 0;
    else
        newInput.x = glm::sign(rawInput.x) * (abs(rawInput.x) - deadZone) / (1 - deadZone);

    if (abs(rawInput.y) < deadZone)
        newInput.y = 0;
    else
        newInput.y = glm::sign(rawInput.y) * (abs(rawInput.y) - deadZone) / (1 - deadZone);

    return newInput;
}