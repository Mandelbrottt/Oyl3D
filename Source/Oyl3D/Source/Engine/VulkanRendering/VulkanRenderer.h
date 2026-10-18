#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Logging/Logging.h>

#include <Rendering/Renderer.h>

namespace Oyl
{
	struct VulkanSwapChain {};

	struct WindowInfo
	{
		VulkanSwapChain swapChain;
	};

	class VulkanRenderer : public IRenderer
	{
	public:
		virtual
		~VulkanRenderer() = default;

		void
		Init() override;

		void
		Shutdown() override;

		void
		RegisterApplicationWindow(IApplicationWindow* a_window) override;

		void
		UnregisterApplicationWindow(IApplicationWindow* a_window) override;

	private:
		void
		CreateVkInstance();

		void
		CreateVkDebugMessenger();

	private:
		ArrayMap<IApplicationWindow*, WindowInfo> m_windowInfos;

		vk::raii::Context m_vkContext;
		vk::raii::Instance m_vkInstance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;
	};
}
