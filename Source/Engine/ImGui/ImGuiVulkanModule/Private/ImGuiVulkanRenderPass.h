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
		ImGuiVulkanRenderPass(const SwapChainHandle& a_swapChain);

		virtual
		~ImGuiVulkanRenderPass();

		void
		Begin(const ICommandBuffer& a_commandBuffer) override;

		void
		Render(const ICommandBuffer& a_commandBuffer) override;

		void
		End(const ICommandBuffer& a_commandBuffer) override;

		ImageHandle
		GetColorAttachment() const override { return m_colorAttachment; }

	private:
		void
		OnSwapChainRecreate(const ISwapChain& a_swapChain);

		void
		CreateRenderTarget(Vector2u a_size);

		SwapChainHandle m_swapChain;

		ImageHandle m_colorAttachment = nullptr;
		RenderTarget m_renderTarget;
	};
}
