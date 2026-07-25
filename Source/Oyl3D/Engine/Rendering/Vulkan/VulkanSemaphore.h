#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanCommandBuffer.h"

#include "Rendering/Semaphore.h"

namespace vk::raii {
	class Semaphore;
}

namespace Oyl::Rendering::Vulkan
{
	using SemaphoreHandle = ImplicitConversionWrapper<SemaphoreHandle, vk::Semaphore>;

	class Device;

	class OYL_RENDERING_API Semaphore : public ISemaphore
	{
	public:
		Semaphore();

		Semaphore(std::nullptr_t)
			: Semaphore() {}

		explicit
		Semaphore(const Device& a_device);

		Semaphore(Semaphore&& a_other) noexcept;
		Semaphore&
		operator =(Semaphore&& a_other) noexcept;

		virtual
		~Semaphore();

		void
		Destroy() override;

		bool
		IsValid() const override;

		SemaphoreHandle
		GetHandle() const;

		const vk::raii::Semaphore&
		GetVkSemaphore() const;

	protected:
		Rendering::SemaphoreHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
