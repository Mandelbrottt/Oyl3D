#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Logging/Logging.h>

#include <Rendering/RenderEngine.h>
#include <Rendering/Renderer.h>

#include "VulkanDevice.h"
#include "VulkanPresentTarget.h"
#include "VulkanSwapChain.h"

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

	private:
		void
		CreateVkInstance();

		void
		CreateVkDebugMessenger();

		PresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) override;

		DeviceHandle
		CreateDevice(IPresentTarget* a_presentTarget) override;

		SwapChainHandle
		CreateSwapChain(IDevice& a_device, IApplicationWindow& a_window, IPresentTarget& a_presentTarget) override;

	private:
		vk::raii::Context m_vkContext;
		vk::raii::Instance m_vkInstance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
	};
}
