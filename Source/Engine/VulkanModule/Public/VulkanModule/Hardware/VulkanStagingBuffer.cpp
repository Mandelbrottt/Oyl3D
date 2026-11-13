#include "VulkanStagingBuffer.h"

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanStagingBuffer::VulkanStagingBuffer(
		const VulkanDevice& a_device,
		const VulkanStagingBufferCreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		CreateBuffer(a_device, a_params);

		if (a_params.pData)
			VulkanStagingBuffer::CopyMemory(a_params.pData, m_size);
	}

	VulkanStagingBuffer::~VulkanStagingBuffer()
	{
		VulkanStagingBuffer::Destroy();
	}

	void
	VulkanStagingBuffer::Destroy()
	{
		if (!IsValid())
			return;

		m_vkBufferMemory.clear();
		m_vkBuffer.clear();
	}

	bool
	VulkanStagingBuffer::IsValid() const
	{
		return *m_vkBuffer
		       && *m_vkBufferMemory;
	}

	bool
	VulkanStagingBuffer::CopyMemory(const void* a_src, uint32 a_size)
	{
		OYL_PROFILE_FUNCTION();

		if (m_size != a_size)
			return false;

		void* copyDst = m_vkBufferMemory.mapMemory(0, a_size);
		std::memcpy(copyDst, a_src, a_size);
		m_vkBufferMemory.unmapMemory();
		return true;
	}

	const vk::raii::Buffer&
	VulkanStagingBuffer::GetVkBuffer() const
	{
		return m_vkBuffer;
	}

	const vk::raii::DeviceMemory&
	VulkanStagingBuffer::GetVkDeviceMemory() const
	{
		return m_vkBufferMemory;
	}

	VulkanStagingBufferId
	VulkanStagingBuffer::GetId() const
	{
		return *m_vkBuffer;
	}

	uint32
	FindMemoryType(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		uint32 a_typeFilter,
		vk::MemoryPropertyFlags a_properties
	)
	{
		auto memoryProperties = a_physicalDevice.getMemoryProperties();

		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			bool isMemorySuitable =
				(a_typeFilter & (1 << i))
				&& (memoryProperties.memoryTypes[i].propertyFlags & a_properties) == a_properties;

			if (isMemorySuitable)
				return i;
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	void
	VulkanStagingBuffer::CreateBuffer(
		const VulkanDevice& a_device,
		const VulkanStagingBufferCreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = a_device.GetVkDevice();
		const auto& physicalDevice = a_device.GetVkPhysicalDevice();

		auto& graphicsQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Graphics);
		auto& transferQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Transfer);

		uint32 indexFamilies[2] { graphicsQueue.GetVkQueueFamilyIndex(), transferQueue.GetVkQueueFamilyIndex() };

		vk::BufferCreateInfo bufferInfo {
			.size = a_params.dataLength,
			.usage = a_params.vkUsage,
			.sharingMode = vk::SharingMode::eConcurrent,
			.queueFamilyIndexCount = (uint32) std::size(indexFamilies),
			.pQueueFamilyIndices = indexFamilies,
		};
		m_vkBuffer = vk::raii::Buffer(device, bufferInfo);
		m_size = a_params.dataLength;

		vk::MemoryRequirements memRequirements = m_vkBuffer.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, a_params.vkProperties)
		};
		m_vkBufferMemory = vk::raii::DeviceMemory(device, allocInfo);
		m_vkBuffer.bindMemory(*m_vkBufferMemory, 0);
	}
}
