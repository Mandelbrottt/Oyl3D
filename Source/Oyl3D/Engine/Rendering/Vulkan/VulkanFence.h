#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Fence.h"

namespace Oyl::Rendering
{
	class VulkanDevice;

	using VulkanFenceId = OpaqueHandleConvertible<FenceId, vk::Fence>;

	class OYL_RENDERING_API VulkanFence : public Fence
	{
	public:
		VulkanFence(nullptr_t) {}

		VulkanFence(const VulkanDevice& a_device);

		VulkanFence(VulkanFence&& a_other) noexcept;
		VulkanFence&
		operator =(VulkanFence&& a_other) noexcept;

		virtual
		~VulkanFence();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Reset() override;

		bool
		Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) override;

		const vk::raii::Fence&
		GetVkFence() const
		{
			return m_fence;
		}

		VulkanFenceId
		GetId() const
		{
			return *m_fence;
		}

		operator VulkanFenceId() const
		{
			return GetId();
		}

	protected:
		FenceId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		const VulkanDevice* m_device = nullptr;
		vk::raii::Fence m_fence = nullptr;
	};

	using VulkanFenceHandle = PImpl<VulkanFence>;
}
