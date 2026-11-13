#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Application/ApplicationWindow.h>

#include <Rendering/RenderContext.h>

#include "Rendering.Vulkan/Hardware/VulkanDevice.h"

namespace Oyl
{
	class RENDERING_VULKAN_EXPORT VulkanRenderContext : public IRenderContext
	{
	public:
		virtual ~VulkanRenderContext() = default;

		void
		Init() override;

		void
		Update() override {}

		void
		Shutdown() override {}

		RenderingApi
		GetRenderingApi() const override { return RenderingApi::Vulkan; }

		HardwarePresentTargetHandle
		CreatePresentTarget(IApplicationWindow &a_window) override;

		RenderDeviceHandle
		CreateDevice(IHardwarePresentTarget *a_presentTarget) override;

		RdoSwapChainHandle
		CreateSwapChain(IRenderDevice &a_device, IApplicationWindow &a_window, IHardwarePresentTarget &a_presentTarget) override;

		SharedPtr<IShaderCompiler>
		CreateShaderCompiler() override;

		const vk::raii::Context &
		GetVkContext() const { return m_vkContext; }

		const vk::raii::Instance &
		GetVkInstance() const { return m_vkInstance; }

	private:
		void
		CreateVkInstance();

		void
		CreateVkDebugMessenger();

	private:
		vk::raii::Context m_vkContext;
		vk::raii::Instance m_vkInstance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
	};
}
