#pragma once

#define _ENABLE_EXTENDED_ALIGNED_STORAGE 1

#include "Oyl3D/Common.h"
#include "Oyl3D/Enums.h"
#include "Oyl3D/Typedefs.h"

#include "Oyl3D/Utils/Logging.h"
#include "Oyl3D/Utils/Refs.h"
#include "Oyl3D/Utils/Timestep.h"

#include "Oyl3D/AssetPaths.h"

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
#include <thread>
#include <future>

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/spline.hpp>

#include <imgui.h>
#include <imguizmo/ImGuizmo.h>

#include <nlohmann/json.hpp>
using nlohmann::json;