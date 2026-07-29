#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Semaphore.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;

	using SemaphoreHandle = OpaqueHandleConvertible<SemaphoreHandle, vk::Semaphore>;

	class OYL_RENDERING_API Semaphore : public ISemaphore
	{
	public:
		Semaphore() = default;

		Semaphore(std::nullptr_t)
			: Semaphore() {}

		explicit
		Semaphore(const Device& a_device);

		Semaphore(Semaphore&& a_other) noexcept;
		Semaphore&
		operator =(Semaphore&& a_other) noexcept;

		virtual
		~Semaphore()
		{
			Semaphore::Destroy();
		};

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Semaphore&
		GetVkSemaphore() const
		{
			return m_semaphore;
		}

		SemaphoreHandle
		GetHandle() const
		{
			return *m_semaphore;
		}

		operator SemaphoreHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::SemaphoreHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		const Device* m_device = nullptr;
		vk::raii::Semaphore m_semaphore = nullptr;
	};
}
