#pragma once

#include <Rendering/ImGui/ImGuiRenderer.h>

namespace Oyl
{
	class ImGuiVulkanRenderer : public IImGuiRenderer
	{
	public:
		void
		Init(IRenderer& a_renderer) override;

		void
		Update(IRenderer& a_renderer) override;

		void
		Shutdown(IRenderer& a_renderer) override;
	};
}