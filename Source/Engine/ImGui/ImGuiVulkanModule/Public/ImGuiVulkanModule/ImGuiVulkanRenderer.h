#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <mutex>

#include <Core/Containers/UniquePtr.h>

#include <Rendering/SwapChain.h>
#include <Rendering/ImGui/ImGuiRenderer.h>

#include "ImGuiVulkanRenderPass.h"

namespace Oyl
{
	class ImGuiVulkanRenderer : public IImGuiRenderer
	{
	public:
		void
		Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow) override;

		void
		Update(Renderer& a_renderer) override;

		void
		Render(Renderer& a_renderer, ICommandBuffer& a_commandBuffer) override;

		void
		Shutdown(Renderer& a_renderer) override;

		void
		SetOnUpdateCallback(Delegate<void()> a_callback) override { m_onUpdate = a_callback; }

	private:
		IApplicationWindow* m_mainWindow = nullptr;
		vk::raii::DescriptorPool m_vkDescriptorPool = nullptr;

		UniquePtr<ImGuiVulkanRenderPass> m_renderPass;

		Delegate<void()> m_onUpdate;

		std::mutex m_renderMutex;
	};
}
