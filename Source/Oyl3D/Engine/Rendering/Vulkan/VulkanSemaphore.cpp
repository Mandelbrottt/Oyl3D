#include "VulkanSemaphore.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct Semaphore::Impl
	{
		vk::raii::Semaphore semaphore = nullptr;
	};

	Semaphore::Semaphore()
		: m_impl(nullptr) {}

	Semaphore::Semaphore(const Device& a_device)
		: m_impl(std::make_unique<Impl>())
	{
		m_impl->semaphore = vk::raii::Semaphore(a_device.GetVkDevice(), vk::SemaphoreCreateInfo {});
	}

	Semaphore::Semaphore(Semaphore&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Semaphore&
	Semaphore::operator=(Semaphore&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	Semaphore::~Semaphore()
	{
		Semaphore::Destroy();
	}

	void
	Semaphore::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->semaphore.clear();
	}

	bool
	Semaphore::IsValid() const
	{
		return m_impl
		       && *m_impl->semaphore;
	}

	SemaphoreHandle
	Semaphore::GetHandle() const
	{
		static SemaphoreHandle handle;

		if (!IsValid())
			return handle;

		return *m_impl->semaphore;
	}

	const vk::raii::Semaphore&
	Semaphore::GetVkSemaphore() const
	{
		return m_impl->semaphore;
	}
}
