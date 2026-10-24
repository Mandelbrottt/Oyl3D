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

		ImageHandle
		GetColorAttachment() const override { return m_colorAttachment; }

	protected:
		void
		Begin(const ICommandBuffer& a_commandBuffer) override;

		void
		Render(const ICommandBuffer& a_commandBuffer) override;

		void
		End(const ICommandBuffer& a_commandBuffer) override;

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
