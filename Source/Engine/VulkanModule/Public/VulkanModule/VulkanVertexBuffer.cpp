#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandQueue.h"
#include "VulkanDevice.h"
#include "VulkanStagingBuffer.h"

namespace Oyl
{
	VulkanVertexBuffer::VulkanVertexBuffer(const VulkanDevice& a_device, const VertexBufferCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_params.vertexData.Size() % a_params.vertexStride == 0);
		m_vertexCount = a_params.vertexData.Size() / a_params.vertexStride;
		m_vertexStride = a_params.vertexStride;

		if (a_params.indexData.Size() != 0 && a_params.indexStride != 0)
		{
			OYL_ASSERT(a_params.indexData.Size() % a_params.indexStride == 0);
			m_indexCount = a_params.indexData.Size() / a_params.indexStride;
			m_indexStride = a_params.indexStride;
		}

		CreateVertexBuffer(a_device, a_params);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VulkanVertexBuffer::Destroy();
	}

	void
	VulkanVertexBuffer::Destroy()
	{
		if (!IsValid())
			return;

		m_buffer.clear();
	}

	static
	uint32
	FindMemoryType(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		uint32 a_typeFilter,
		vk::MemoryPropertyFlags a_properties
	);

	static
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
	CreateBuffer(
		const VulkanDevice& a_device,
		vk::DeviceSize a_size,
		vk::BufferUsageFlags a_usage,
		vk::MemoryPropertyFlags a_properties
	);

	static
	void
	CopyBuffer(
		const VulkanDevice& a_device,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	);

	void
	VulkanVertexBuffer::CreateVertexBuffer(const VulkanDevice& a_device, const VertexBufferCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		auto vertexData = a_params.vertexData;
		auto vertexLength = a_params.vertexData.Size();
		auto indexData = a_params.indexData;
		auto indexLength = a_params.indexData.Size();

		// Combine vertex and index data into one contiguous buffer
		std::vector<byte> combinedDataBuffer;
		combinedDataBuffer.reserve(vertexLength + indexLength);
		if (indexLength > 0)
			combinedDataBuffer.insert(combinedDataBuffer.end(), &indexData[0], &indexData.Data()[indexLength]);
		combinedDataBuffer.insert(combinedDataBuffer.end(), &vertexData[0], &vertexData.Data()[vertexLength]);

		auto stagingBuffer = VulkanStagingBuffer(
			a_device,
			{
				.pData = combinedDataBuffer.data(),
				.dataLength = (uint32) combinedDataBuffer.size(),
				.vkUsage = vk::BufferUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eHostVisible
				                | vk::MemoryPropertyFlagBits::eHostCoherent
			}
		);

		// Copy data in staging buffer to main buffer
		std::tie(m_buffer, m_bufferMemory) =
			CreateBuffer(
				a_device,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eIndexBuffer
				| vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

		CopyBuffer(a_device, stagingBuffer.GetVkBuffer(), m_buffer, combinedDataBuffer.size());
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

	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory>
	CreateBuffer(
		const VulkanDevice& a_device,
		vk::DeviceSize a_size,
		vk::BufferUsageFlags a_usage,
		vk::MemoryPropertyFlags a_properties
	)
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = a_device.GetVkDevice();
		const auto& physicalDevice = a_device.GetVkPhysicalDevice();

		vk::BufferCreateInfo bufferInfo { .size = a_size, .usage = a_usage, .sharingMode = vk::SharingMode::eExclusive };
		vk::raii::Buffer buffer = vk::raii::Buffer(device, bufferInfo);

		vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo { .allocationSize = memRequirements.size, .memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, a_properties) };
		vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(device, allocInfo);
		buffer.bindMemory(*bufferMemory, 0);

		return { std::move(buffer), std::move(bufferMemory) };
	}

	void
	CopyBuffer(
		const VulkanDevice& a_device,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	)
	{
		OYL_PROFILE_FUNCTION();

		auto commandPool = VulkanCommandPool(a_device, { .commandQueueFlags = CommandQueueFlagBits::Transfer });
		auto commandBuffer = VulkanCommandBuffer(a_device, { .commandPool = commandPool });

		auto& vkCommandBuffer = commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		vkCommandBuffer.copyBuffer(*a_srcBuffer, *a_dstBuffer, vk::BufferCopy(0, 0, a_size));
		vkCommandBuffer.end();

		auto& transferQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Transfer);

		transferQueue.Submit({ .commandBuffer = commandBuffer });
		transferQueue.WaitUntilIdle();
	}
}
