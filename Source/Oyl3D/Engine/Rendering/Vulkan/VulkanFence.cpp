#include "VulkanFence.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct Fence::Impl
	{
		vk::raii::Fence fence = nullptr;
	};

	Fence::Fence()
		: m_impl(nullptr) {}

	Fence::Fence(const Device& a_device)
		: m_impl(std::make_unique<Impl>())
	{
		m_impl->fence = vk::raii::Fence(
			a_device.GetVkDevice(),
			vk::FenceCreateInfo {
				.flags = vk::FenceCreateFlagBits::eSignaled
			}
		);
	}

	Fence::Fence(Fence&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Fence&
	Fence::operator=(Fence&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
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

		m_impl->fence.clear();
	}

	bool
	Fence::IsValid() const
	{
		return m_impl
		       && *m_impl->fence;
	}

	FenceHandle
	Fence::GetHandle() const
	{
		if (!IsValid())
			return nullptr;

		return *m_impl->fence;
	}

	const vk::raii::Fence&
	Fence::GetVkFence() const
	{
		return m_impl->fence;
	}
}
