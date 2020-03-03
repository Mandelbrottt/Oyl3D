#pragma once

#include "Interpolation.h"

namespace oyl::Interpolation
{
    // Polynomial easing functions adapted from https://gist.github.com/gre/1650294
    float linear(float t)
    {
        return t;
    }

    float pow2in(float t)
    {
        return t * t;
    }

    float pow2out(float t)
    {
        return t * (2 - t);
    }

    float pow2inout(float t)
    {
        return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
    }

    float pow3in(float t)
    {
        return t * t * t;
    }

    float pow3out(float t)
    {
        return (--t) * t * t + 1;
    }

    float pow3inout(float t)
    {
        return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
    }
}
