#include "ImGuiVulkanRenderer.h"

#include <Core/Logging/Logging.h>
#include <Core/Profiling/Profiling.h>

namespace Oyl
{
	void
	ImGuiVulkanRenderer::Init(IRenderer& a_renderer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer);

		LogWarning("ImGuiRenderer::Init()!");
	}

	void
	ImGuiVulkanRenderer::Update(IRenderer& a_renderer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer);
	}

	void
	ImGuiVulkanRenderer::Shutdown(IRenderer& a_renderer)
	{
		OYL_PROFILE_FUNCTION();
		OYL_UNUSED(a_renderer);
	}
}
