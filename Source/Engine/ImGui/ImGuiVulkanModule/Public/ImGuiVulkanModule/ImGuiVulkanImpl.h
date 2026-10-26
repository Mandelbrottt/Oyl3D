#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Core/Containers/UniquePtr.h>

#include <ImGuiModule/IImGuiImpl.h>

#include <Rendering/SwapChain.h>
#include <Rendering/ImGui/IImGuiRenderer.h>

#include "ImGuiVulkanRenderPass.h"

namespace Oyl
{
	class ImGuiVulkanImpl : public IImGuiImpl
	{
	public:
		void
		Init(IImGuiRenderer& a_imGuiRenderer, IApplicationWindow& a_mainWindow) override;

		void
		Update() override;

		void
		Render(ICommandBuffer& a_commandBuffer) override;

		void
		Shutdown() override;

		const ImageHandle&
		GetRenderedImageHandle() const override
		{
			return m_renderPass->GetColorAttachment();
		}

	private:
		void
		CreateDescriptorPools();

		void
		InitImGuiVulkan();

		void
		OnSwapChainRecreate(const ISwapChain& a_swapChain);

	private:
		IImGuiRenderer* m_imGuiRenderer = nullptr;
		IApplicationWindow* m_mainWindow = nullptr;

		vk::raii::DescriptorPool m_vkDescriptorPool = nullptr;

		UniquePtr<ImGuiVulkanRenderPass> m_renderPass;
	};
}
