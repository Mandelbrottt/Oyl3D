#pragma once

namespace oyl::Interpolation
{
    enum class Type
    {
        Linear, Cubic, Spline = Cubic
    };

    using EaseFn = float(*)(float);

    static float linear(float t);

    static float pow2in(float t);
    static float pow2out(float t);
    static float pow2inout(float t);

    static float pow3in(float t);
    static float pow3out(float t);
    static float pow3inout(float t);
}

#include "Interpolation.inl"