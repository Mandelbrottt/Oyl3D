#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandQueue.h"

#include "Rendering/Device.h"

namespace Oyl::Rendering::Vulkan
{
	class CommandQueue;

	class OYL_RENDERING_API Device : public IDevice
	{
	public:
		Device();

		struct CreateParams
		{
			const IWindow& window;

			CommandQueueFlags commandQueueFlags;

			const char* const* ppRequiredDeviceExtensionsData;
			size_t requiredDeviceExtensionsLength;
		};

		explicit
		Device(const CreateParams& a_params);

		Device(Device&& a_other) noexcept;
		Device&
		operator =(Device&& a_other) noexcept;

		virtual
		~Device();

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

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
