#pragma once

#define _ENABLE_EXTENDED_ALIGNED_STORAGE 1

#if !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(OYL_PLATFORM_WINDOWS)
    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <future>
#include <filesystem>

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <string>
#include <sstream>

#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <bitset>

#include <entt/single_include/entt/entt.hpp>

#define GLM_FORCE_ALIGNED_GENTYPES  1
#define GLM_FORCE_SWIZZLE 1
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/type_ptr.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS 1
#include <imgui.h>
#include <imguizmo/ImGuizmo.h>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "Oyl3D/Common.h"
#include "Oyl3D/Typedefs.h"

#include "Oyl3D/AssetPaths.h"

#include "Oyl3D/Utils/Time.h"
#include "Oyl3D/Utils/Logging.h"
#include "Oyl3D/Utils/Refs.h"
#include "Oyl3D/Utils/Timestep.h"
