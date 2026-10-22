#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Application/ApplicationWindow.h>

#include <Rendering/Renderer.h>

#include "VulkanDevice.h"

namespace Oyl
{
	class VulkanRenderer : public IRenderer
	{
	public:
		virtual
		~VulkanRenderer() = default;

		void
		Init() override;

		void
		Update() override {}

		void
		Shutdown() override {}

		RenderingApi
		GetRenderingApi() const override { return RenderingApi::Vulkan; }

		PresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) override;

		DeviceHandle
		CreateDevice(IPresentTarget* a_presentTarget) override;

		SwapChainHandle
		CreateSwapChain(IDevice& a_device, IApplicationWindow& a_window, IPresentTarget& a_presentTarget) override;

		SharedPtr<IShaderCompiler>
		CreateShaderCompiler() override;

		const vk::raii::Context&
		GetVkContext() const { return m_vkContext; }

		const vk::raii::Instance&
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
