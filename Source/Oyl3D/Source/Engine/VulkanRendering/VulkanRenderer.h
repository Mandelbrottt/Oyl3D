#pragma once

#include <Core/Logging/Logging.h>

#include <Rendering/Renderer.h>

namespace Oyl
{
	class VulkanRenderer : public IRenderer
	{
	public:
		VulkanRenderer()
		{
			LogWarning("VulkanRenderer!");
		}

		virtual
		~VulkanRenderer() = default;

	private:
	};
}
