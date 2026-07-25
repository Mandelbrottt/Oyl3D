#include "VulkanFence.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct Fence::Impl
	{
		vk::raii::Fence fence = nullptr;
	};

	Semaphore::Semaphore()
		: m_impl(nullptr) {}

	Semaphore::Semaphore(const Device& a_device)
		: m_impl(std::make_unique<Impl>())
	{
		m_impl->fence = vk::raii::Semaphore(a_device.GetVkDevice(), {});
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

		m_impl->fence.clear();
	}

	bool
	Semaphore::IsValid() const
	{
		return m_impl
		       && *m_impl->fence;
	}

	const SemaphoreHandle&
	Semaphore::GetHandle() const
	{
		if (!IsValid())
			return nullptr;

		return *m_impl->fence;
	}
}
