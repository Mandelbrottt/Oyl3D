#pragma once

#include <cstdint>
#include <memory>
#include <entt/entt.hpp>

namespace oyl
{
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using uint = u32;

    using f32 = float;
    static_assert(sizeof(float) == 4, "float should be 32 bits wide!");
    using f64 = double;
    static_assert(sizeof(double) == 8, "double should be 64 bits wide!");

    using Entity = entt::entity;

    using CacheAlias = std::string;
}
