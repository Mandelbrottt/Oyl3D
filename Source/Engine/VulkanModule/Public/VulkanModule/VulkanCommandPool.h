#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/CommandPool.h"

namespace Oyl
{
	class VulkanDevice;

	class VULKANMODULE_EXPORT VulkanCommandPool : public ICommandPool
	{
	public:
		VulkanCommandPool(nullptr_t) {}

		VulkanCommandPool(const VulkanDevice& a_device, const CommandPoolCreateParams& a_params);

		DEFAULT_MOVE(VulkanCommandPool);

		virtual
		~VulkanCommandPool();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::CommandPool&
		GetVkCommandPool() const;

	private:
		vk::raii::CommandPool m_commandPool = nullptr;
	};
}
