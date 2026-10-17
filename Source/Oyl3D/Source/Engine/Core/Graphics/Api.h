#pragma once

#include "Core/Types/Primitives.h"

namespace Oyl
{
	enum class GraphicsApi : uint32
	{
		None,
		OpenGL,
		Vulkan,
		DirectX11,
		DirectX12,
		Metal,
		WebGL,
	};
}