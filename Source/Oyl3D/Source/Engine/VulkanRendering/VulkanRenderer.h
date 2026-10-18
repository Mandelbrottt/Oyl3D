#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Logging/Logging.h>

#include <Rendering/Renderer.h>

#include "VulkanDevice.h"
#include "VulkanPresentTarget.h"
#include "VulkanSwapChain.h"

namespace Oyl
{
	struct WindowInfo
	{
		VulkanPresentTarget presentTarget = nullptr;
		VulkanSwapChain swapChain = nullptr;
	};

	class VulkanRenderer : public IRenderer
	{
	public:
		virtual
		~VulkanRenderer() = default;

		void
		Init(IApplicationWindow* a_mainWindow) override;

		void
		Update() override;

		void
		Shutdown() override;

		void
		RegisterApplicationWindow(IApplicationWindow& a_window) override;

		void
		UnregisterApplicationWindow(IApplicationWindow& a_window) override;

	private:
		void
		CreateVkInstance();

		void
		CreateVkDebugMessenger();

		VulkanPresentTarget
		CreatePresentTarget(IApplicationWindow& a_window);

		VulkanSwapChain
		CreateSwapChain(IApplicationWindow& a_window, VulkanPresentTarget& a_presentTarget);

	private:
		ArrayMap<IApplicationWindow*, WindowInfo> m_windowInfos;

		vk::raii::Context m_vkContext;
		vk::raii::Instance m_vkInstance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

		VulkanDevice m_device = nullptr;
	};
}
