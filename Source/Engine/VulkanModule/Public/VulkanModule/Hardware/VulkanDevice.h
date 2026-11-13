#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <cstddef>

#include <Core/Containers/Array.h>
#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/String.h>

#include <Rendering/Hardware/RenderDevice.h>

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandQueue.h"
#include "VulkanFence.h"
#include "VulkanImage.h"
#include "VulkanPresentTarget.h"
#include "VulkanSemaphore.h"
#include "VulkanShader.h"
#include "VulkanVertexBuffer.h"

namespace Oyl
{
	struct VulkanDeviceCreateParams
	{
		const VulkanPresentTarget* presentTarget;
		CommandQueueFlags commandQueueFlags;
		ArrayProxy<const char*> requiredDeviceExtensions;
	};

	class VULKANMODULE_EXPORT VulkanDevice : public IRenderDevice
	{
	public:
		VulkanDevice(nullptr_t) {}

		explicit
		VulkanDevice(const vk::raii::Instance& a_vkInstance, const VulkanDeviceCreateParams& a_params);

		DEFAULT_MOVE(VulkanDevice);

		virtual
		~VulkanDevice();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkDevice; }

		void
		WaitUntilIdle() const override;

		RdoCommandBufferHandle
		CreateCommandBuffer(const RdoCommandBufferCreateParams& a_params) const override;

		RdoCommandPoolHandle
		CreateCommandPool(const RdoCommandPoolCreateParams& a_params) const override;

		RdoImageHandle
		CreateImage(const RdoImageCreateParams& a_params) const override;

		RdoShaderHandle
		CreateShader(const RdoShaderCreateParams& a_params) const override;

		RdoVertexBufferHandle
		CreateVertexBuffer(const RdoVertexBufferCreateParams& a_params) const override;

		RdoUniformBufferHandle
		CreateUniformBuffer(const RdoUniformBufferCreateParams& a_params) const override;

		RdoSemaphoreHandle
		CreateSemaphore() const override;

		RdoFenceHandle
		CreateFence() const override;

		const VulkanCommandQueue*
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
