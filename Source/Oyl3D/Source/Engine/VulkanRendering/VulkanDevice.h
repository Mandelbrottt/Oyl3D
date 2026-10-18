#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <cstddef>

#include <Core/Containers/Array.h>
#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/String.h>

#include <Rendering/Device.h>

#include "VulkanCommandQueue.h"
#include "VulkanPresentTarget.h"
#include "VulkanRenderingEnums.h"

namespace Oyl
{
	struct VulkanDeviceCreateParams
	{
		const VulkanPresentTarget* presentTarget;
		CommandQueueFlags commandQueueFlags;
		Array<const char*> requiredDeviceExtensions;
	};

	class VulkanDevice : public IDevice
	{
	public:
		VulkanDevice(nullptr_t) {}

		explicit
		VulkanDevice(const vk::raii::Instance& a_vkInstance, const VulkanDeviceCreateParams& a_params);

		DEFAULT_MOVE(VulkanDevice);
		NO_COPY(VulkanDevice);

		virtual
		~VulkanDevice();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkDevice; }

		void
		WaitUntilIdle() const override;

		const ICommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const override;

		const vk::raii::PhysicalDevice&
		GetVkPhysicalDevice() const { return m_vkPhysicalDevice; }

		const vk::raii::Device&
		GetVkDevice() const { return m_vkDevice; }

	private:
		void
		PickPhysicalDevice(const vk::raii::Instance& a_vkInstance, CommandQueueFlags a_queueFlags);

		void
		CreateLogicalDevice(const VulkanPresentTarget* a_presentTarget);

		void
		CreateCommandQueues();

	private:
		Array<String> m_requiredDeviceExtensions;

		vk::raii::PhysicalDevice m_vkPhysicalDevice = nullptr;
		vk::raii::Device m_vkDevice = nullptr;

		ArrayMap<CommandQueueFlagBits, uint32> m_queueFamilyIndices;
		ArrayMap<CommandQueueFlagBits, VulkanCommandQueue> m_queues;
	};
}
