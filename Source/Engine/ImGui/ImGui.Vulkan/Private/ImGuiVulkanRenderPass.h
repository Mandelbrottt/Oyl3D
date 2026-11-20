#pragma once

#include <Rendering/RenderPass/ColorOutputRenderPass.h>
#include <Rendering/RenderPass/RenderPass.h>

namespace Oyl
{
	class ImGuiVulkanRenderPass
		: public IColorOutputRenderPass,
		  public RenderPass<ImGuiVulkanRenderPass>
	{
	public:
		explicit
		ImGuiVulkanRenderPass(Vector2u a_size);

		virtual
		~ImGuiVulkanRenderPass() = default;

		const RdoImageHandle&
		GetColorAttachment() const override { return m_colorAttachment; }

		void
		OnViewPortResize(Vector2u a_size) override;

		void
		SetGameViewPortColorAttachment(const RdoImageHandle& a_imageHandle)
		{
			if (m_gameViewPortColorAttachment == a_imageHandle)
				return;
			m_gameViewPortColorAttachment = a_imageHandle;
		}

		void
		Render(const IRdoCommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

		RdoImageHandle m_gameViewPortColorAttachment = nullptr;

		RdoImageHandle m_colorAttachment = nullptr;
		RenderTarget m_renderTarget;
	};
}
