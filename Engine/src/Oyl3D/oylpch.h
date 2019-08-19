#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <thread>
#include <future>

#include <string>
#include <sstream>

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Typedefs.h"
#include "Common.h"

#include "System/Logging.h"

#include "Enums.h"

#ifdef OYL_PLATFORM_WINDOWS
#	include <windows.h>
#endif