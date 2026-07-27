#include "VulkanFence.h"

#include "VulkanDevice.h"

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

	void
	Fence::Reset()
	{
		const auto& vkFence = GetVkFence();
		const auto& vkDevice = vkFence.getDevice();

		vkDevice.resetFences(*vkFence);
	}

	bool
	Fence::Wait(uint64 a_timeout)
	{
		const auto& vkFence = GetVkFence();
		const auto& vkDevice = vkFence.getDevice();

		auto result = vkDevice.waitForFences(*vkFence, vk::True, a_timeout);
		return result != vk::Result::eSuccess;
	}
}
