#include "VulkanFence.h"

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	Fence::Fence(const Device& a_device)
		: m_device(&a_device),
		  m_fence(
			  vk::raii::Fence(
				  a_device.GetVkDevice(),
				  vk::FenceCreateInfo {
					  .flags = vk::FenceCreateFlagBits::eSignaled
				  }
			  )
		  ) {}

	Fence::Fence(Fence&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Fence&
	Fence::operator=(Fence&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_device, a_other.m_device);
			std::swap(m_fence, a_other.m_fence);
		}
		return *this;
	}

	Fence::~Fence()
	{
		Fence::Destroy();
	}

	void
	Fence::Destroy()
	{
		if (!IsValid())
			return;

		m_fence.clear();
	}

	bool
	Fence::IsValid() const
	{
		return m_device && *m_fence;
	}

	void
	Fence::Reset()
	{
		const auto& vkFence = GetVkFence();
		const auto& vkDevice = m_device->GetVkDevice();

		vkDevice.resetFences(*vkFence);
	}

	bool
	Fence::Wait(uint64 a_timeout)
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkFence = GetVkFence();
		const auto& vkDevice = m_device->GetVkDevice();

		auto result = vkDevice.waitForFences(*vkFence, vk::True, a_timeout);
		return result != vk::Result::eSuccess;
	}
}
