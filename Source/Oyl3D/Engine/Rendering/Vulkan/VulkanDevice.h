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

namespace Oyl
{
	class Window;
}

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API Device : public Rendering::Device
	{
	public:
		Device();

		struct CreateParams
		{
			const vk::raii::Instance& instance;
			const Window* window;

			const char* const* ppRequiredDeviceExtensionsData;
			size_t requiredDeviceExtensionsLength;
		};

		explicit
		Device(const CreateParams& a_params);

		Device(const Device& a_other) = delete;
		Device&
		operator =(const Device& a_other) = delete;

		Device(Device&& a_other) noexcept;
		Device&
		operator =(Device&& a_other) noexcept;

		virtual
		~Device();

		bool
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Device&
		GetVkDevice() const;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const;

		const vk::raii::SurfaceKHR&
		GetVkSurface() const;

		const vk::raii::Queue&
		GetVkGraphicsQueue() const;

		uint32
		GetVkGraphicsQueueIndex() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
