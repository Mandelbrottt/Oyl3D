#include "ImGuiVulkanRenderPass.h"

#include <ImGui/backends/imgui_impl_vulkan.h>

#include <Rendering/RenderEngine.h>

#include <VulkanModule/VulkanCommandBuffer.h>

namespace Oyl
{
	ImGuiVulkanRenderPass::ImGuiVulkanRenderPass(const SwapChainHandle& a_swapChain)
		: m_swapChain(a_swapChain)
	{
		CreateRenderTarget(m_swapChain->GetSize());
		m_swapChain->AddOnRecreateCallback({ this, &ImGuiVulkanRenderPass::OnSwapChainRecreate });
	}

	ImGuiVulkanRenderPass::~ImGuiVulkanRenderPass()
	{
		m_swapChain->RemoveOnRecreateCallback({ this, &ImGuiVulkanRenderPass::OnSwapChainRecreate });
	}

	void
	ImGuiVulkanRenderPass::Begin(const ICommandBuffer& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);
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
	ImGuiVulkanRenderPass::OnSwapChainRecreate(const ISwapChain& a_swapChain)
	{
        ImGui_ImplVulkan_SetMinImageCount(a_swapChain.GetMinImageCount());
		CreateRenderTarget(a_swapChain.GetSize());
	}

	void
	ImGuiVulkanRenderPass::CreateRenderTarget(Vector2u a_size)
	{
		auto& device = RenderEngine::Get().GetDevice();
		m_colorAttachment = device.CreateImage(
			{
				.size = a_size,
				.format = ImageFormat::RGBA_8_SRGB,
				.usageFlags = ImageUsageFlagBits::ColorAttachment | ImageUsageFlagBits::TransferSrc,
				.layout = ImageLayout::ColorAttachment
			}
		);
		m_renderTarget = RenderTarget({ .colorImages = &*m_colorAttachment });
	}
}
