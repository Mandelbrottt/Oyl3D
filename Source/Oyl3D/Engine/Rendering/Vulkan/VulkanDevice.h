#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandQueue.h"

#include "Rendering/Device.h"

namespace Oyl::Glfw
{
	class Window;
}

namespace Oyl::Rendering::Vulkan
{
	class CommandQueue;

	class OYL_RENDERING_API DeviceImpl : public Rendering::DeviceImpl
	{
		struct DeviceImplTag {};

	public:
		struct CreateParams
		{
			const IWindow& window;

			CommandQueueFlags commandQueueFlags;

			const char* const* ppRequiredDeviceExtensionsData;
			size_t requiredDeviceExtensionsLength;
		};

		static
		PImpl<DeviceImpl>
		Create(const CreateParams& a_params);

		explicit
		DeviceImpl(DeviceImplTag, const CreateParams& a_params);

		virtual
		~DeviceImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const IWindow*
		GetWindow() const override;

		const CommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const override;

		const vk::raii::Device&
		GetVkDevice() const;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const;

		const vk::raii::SurfaceKHR&
		GetVkSurface() const;

		void
		WaitUntilIdle() const override;

	private:
		void
		CreateInstance();

		void
		CreateDebugMessenger();

		void
		CreateSurface();

		void
		PickPhysicalDevice(CommandQueueFlags a_queueFlags);

		void
		CreateLogicalDevice();

		void
		CreateCommandQueues();

	private:
		const Glfw::Window* m_window = nullptr;

		std::vector<std::string> m_requiredDeviceExtensions;

		vk::raii::Context m_context;
		vk::raii::Instance m_instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

		vk::raii::SurfaceKHR m_surface = nullptr;

		vk::raii::PhysicalDevice m_physicalDevice = nullptr;
		vk::raii::Device m_device = nullptr;

		std::unordered_map<CommandQueueFlagBits, uint32> m_queueFamilyIndices;
		std::unordered_map<CommandQueueFlagBits, CommandQueue> m_queues;
	};

	using Device = PImpl<DeviceImpl>;
}
