#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Semaphore.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;

	using SemaphoreHandle = OpaqueHandleConvertible<SemaphoreHandle, vk::Semaphore>;

	class OYL_RENDERING_API SemaphoreImpl : public Rendering::SemaphoreImpl
	{
	public:
		SemaphoreImpl(nullptr_t) {}

		SemaphoreImpl(const DeviceImpl& a_device);

		SemaphoreImpl(SemaphoreImpl&& a_other) noexcept;
		SemaphoreImpl&
		operator =(SemaphoreImpl&& a_other) noexcept;

		virtual
		~SemaphoreImpl()
		{
			SemaphoreImpl::Destroy();
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
		const DeviceImpl* m_device = nullptr;
		vk::raii::Semaphore m_semaphore = nullptr;
	};

	using Semaphore = PImpl<SemaphoreImpl>;
}
