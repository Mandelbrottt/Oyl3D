#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Fence.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanFenceId = IConvertibleDeviceObjectId<FenceId, vk::Fence>;

	class VULKANMODULE_EXPORT VulkanFence : public IFence
	{
	public:
		VulkanFence(nullptr_t) {}

		explicit
		VulkanFence(const VulkanDevice& a_device);

		DEFAULT_MOVE(VulkanFence);

		virtual
		~VulkanFence();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Reset() override;

		bool
		Wait(uint64 a_timeout = INT64_MAX) override;

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
}
