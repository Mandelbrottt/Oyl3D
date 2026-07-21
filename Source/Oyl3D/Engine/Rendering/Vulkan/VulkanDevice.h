#pragma once

#include "Rendering/Device.h"

namespace vk::raii
{
	class Device;
	class Instance;
	class PhysicalDevice;
	class Queue;
	class SurfaceKHR;
}

namespace Oyl::Rendering::Vulkan
{
	struct DeviceParams
	{
		const vk::raii::Instance& instance;
		const vk::raii::SurfaceKHR& surface;

		const char* const* ppRequiredDeviceExtensionsData;
		size_t requiredDeviceExtensionsLength;
	};

	class OYL_RENDERING_API Device : public Rendering::Device
	{
	public:
		Device();

		explicit
		Device(const DeviceParams& a_params);

		virtual
		~Device();

		Device(const Device& a_other) = delete;
		virtual
		Device&
		operator =(const Device& a_other) = delete;

		Device(Device&& a_other) noexcept;
		virtual
		Device&
		operator =(Device&& a_other) noexcept;

		bool
		Destroy() override;

		const vk::raii::Device&
		GetVkDevice() const;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const;

		const vk::raii::Queue&
		GetVkGraphicsQueue() const;

		uint32
		GetVkGraphicsQueueIndex() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
