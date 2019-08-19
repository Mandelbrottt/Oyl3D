#pragma once

#include <cstdint>
#include <memory>

using uint = uint32_t;
namespace oyl {
template<class T> using ref = std::shared_ptr<T>;
template<class T> using uniqueRef = std::unique_ptr<T>;
}