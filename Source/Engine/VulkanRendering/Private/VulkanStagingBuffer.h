#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/StagingBuffer.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanStagingBufferId = IConvertibleDeviceObjectId<StagingBufferId, vk::Buffer>;

	struct VulkanStagingBufferCreateParams
	{
		const byte* pData = nullptr;

		uint32 dataLength;
		vk::BufferUsageFlags vkUsage;
		vk::MemoryPropertyFlags vkProperties;
	};

	class VulkanStagingBuffer : public IStagingBuffer
	{
	public:
		VulkanStagingBuffer(nullptr_t) {}

		explicit
		VulkanStagingBuffer(const VulkanDevice& a_device, const VulkanStagingBufferCreateParams& a_params);

		DEFAULT_MOVE(VulkanStagingBuffer);

		virtual
		~VulkanStagingBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override;

		bool
		CopyMemory(const void* a_src, uint32 a_size) override;

		const vk::raii::Buffer&
		GetVkBuffer() const;

		const vk::raii::DeviceMemory&
		GetVkDeviceMemory() const;

		VulkanStagingBufferId
		GetId() const;

		operator VulkanStagingBufferId() const
		{
			return GetId();
		}

	protected:
		StagingBufferId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		void
		CreateBuffer(const VulkanDevice& a_device, const VulkanStagingBufferCreateParams& a_params);

	private:
		vk::raii::Buffer m_vkBuffer = nullptr;
		vk::raii::DeviceMemory m_vkBufferMemory = nullptr;

		uint32 m_size = 0;
	};
}
