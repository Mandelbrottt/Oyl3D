#include "ImGuiVulkanRenderPass.h"

#include <ImGui/backends/imgui_impl_vulkan.h>

#include <Rendering/RenderEngine.h>

#include <VulkanModule/VulkanCommandBuffer.h>

namespace Oyl
{
	ImGuiVulkanRenderPass::ImGuiVulkanRenderPass(Vector2u a_size)
	{
		CreateRenderTarget(a_size);
	}

	void
	ImGuiVulkanRenderPass::OnViewPortResize(Vector2u a_size)
	{
		CreateRenderTarget(a_size);
	}

	void
	ImGuiVulkanRenderPass::Begin(const ICommandBuffer& a_commandBuffer)
	{
		if (m_gameViewPortColorAttachment)
		{
			a_commandBuffer.TransitionImageLayout(*m_gameViewPortColorAttachment, ImageLayout::TransferSource);
			a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::TransferDest);
			a_commandBuffer.BlitImage(*m_gameViewPortColorAttachment, *m_colorAttachment, ImageFilter::Nearest);
			a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);
		} else
		{
			a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);
		}

		auto size = m_colorAttachment->GetSize();
		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.BeginRendering(m_renderTarget);
	}

	void
	ImGuiVulkanRenderPass::Render(const ICommandBuffer& a_commandBuffer)
	{
		ImGui::Render();
		auto* drawData = ImGui::GetDrawData();

		auto& vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(a_commandBuffer);
		ImGui_ImplVulkan_RenderDrawData(drawData, *vulkanCommandBuffer.GetVkCommandBuffer());
	}

	void
	ImGuiVulkanRenderPass::End(const ICommandBuffer& a_commandBuffer)
	{
		a_commandBuffer.EndRendering();
		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::TransferSource);
	}

	void
	ImGuiVulkanRenderPass::CreateRenderTarget(Vector2u a_size)
	{
		auto& device = RenderEngine::Get().GetDevice();
		m_colorAttachment = device.CreateImage(
			{
				.size = a_size,
				.format = ImageFormat::RGBA_8_SRGB,
				.usageFlags = ImageUsageFlagBits::ColorAttachment | ImageUsageFlagBits::TransferSrc | ImageUsageFlagBits::TransferDst,
				.layout = ImageLayout::ColorAttachment
			}
		);
		m_renderTarget = RenderTarget({ .colorImages = &*m_colorAttachment });
	}
}
