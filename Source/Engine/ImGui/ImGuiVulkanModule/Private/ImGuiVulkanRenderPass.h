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

		ImageHandle
		GetColorAttachment() const override { return m_colorAttachment; }

		void
		OnViewPortResize(Vector2u a_size) override;

		void
		SetGameViewPortColorAttachment(const ImageHandle& a_imageHandle)
		{
			if (m_gameViewPortColorAttachment == a_imageHandle)
				return;
			m_gameViewPortColorAttachment = a_imageHandle;
		}

	protected:
		void
		Begin(const ICommandBuffer& a_commandBuffer) override;

		void
		Render(const ICommandBuffer& a_commandBuffer) override;

		void
		End(const ICommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

		ImageHandle m_gameViewPortColorAttachment = nullptr;

		ImageHandle m_colorAttachment = nullptr;
		RenderTarget m_renderTarget;
	};
}
