#include "VulkanFence.h"

namespace Oyl::Rendering::Vulkan
{
	Fence::Fence(const Device& a_device)
		: m_fence(
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
			m_fence = std::move(a_other.m_fence);
		}
		return *this;
	}

	void
	Fence::Destroy()
	{
		if (!IsValid())
			return;

		m_fence.clear();
	}
}
