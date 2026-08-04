#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandQueue.h"
#include "VulkanDevice.h"
#include "VulkanStagingBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferImpl::Impl
	{
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;

		uint32 vertexCount;
		uint32 vertexStride;

		uint32 indexCount = 0;
		uint32 indexStride = 0;

		void
		CreateVertexBuffer(const DeviceImpl& a_device, const CreateParams& a_params);
	};

	VertexBufferImpl::VertexBufferImpl(nullptr_t)
		: m_impl(nullptr) {}

	VertexBufferImpl::VertexBufferImpl(const DeviceImpl& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_params.vertexData.Size() % a_params.vertexStride == 0);
		m_impl->vertexCount = a_params.vertexData.Size() / a_params.vertexStride;
		m_impl->vertexStride = a_params.vertexStride;

		if (a_params.indexData.Size() != 0 && a_params.indexStride != 0)
		{
			OYL_ASSERT(a_params.indexData.Size() % a_params.indexStride == 0);
			m_impl->indexCount = a_params.indexData.Size() / a_params.indexStride;
			m_impl->indexStride = a_params.indexStride;
		}

		m_impl->CreateVertexBuffer(a_device, a_params);
	}

	VertexBufferImpl::VertexBufferImpl(VertexBufferImpl&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VertexBufferImpl&
	VertexBufferImpl::operator=(VertexBufferImpl&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	VertexBufferImpl::~VertexBufferImpl()
	{
		VertexBufferImpl::Destroy();
	}

	void
	VertexBufferImpl::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->buffer.clear();
	}

	bool
	VertexBufferImpl::IsValid() const
	{
		return m_impl
		       && *m_impl->buffer;
	}

	uint32
	VertexBufferImpl::GetVertexCount() const
	{
		return m_impl->vertexCount;
	}

	uint32
	VertexBufferImpl::GetVertexStride() const
	{
		return m_impl->vertexStride;
	}

	uint32
	VertexBufferImpl::GetVertexDataOffset() const
	{
		if (m_impl->indexCount == 0)
			return 0;

		return m_impl->indexCount * m_impl->indexStride;
	}

	uint32
	VertexBufferImpl::GetIndexCount() const
	{
		return m_impl->indexCount;
	}

	uint32
	VertexBufferImpl::GetIndexStride() const
	{
		return m_impl->indexStride;
	}

	const vk::raii::Buffer&
	VertexBufferImpl::GetVkBuffer() const
	{
		return m_impl->buffer;
	}

	VertexBufferHandle
	VertexBufferImpl::GetHandle() const
	{
		return *m_impl->buffer;
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
		const DeviceImpl& a_device,
		vk::DeviceSize a_size,
		vk::BufferUsageFlags a_usage,
		vk::MemoryPropertyFlags a_properties
	);

	static
	void
	CopyBuffer(
		const DeviceImpl& a_device,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	);

	void
	VertexBufferImpl::Impl::CreateVertexBuffer(const DeviceImpl& a_device, const CreateParams& a_params)
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

		auto stagingBuffer = StagingBuffer(
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
		std::tie(buffer, bufferMemory) =
			CreateBuffer(
				a_device,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eIndexBuffer
				| vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

		CopyBuffer(a_device, stagingBuffer.GetVkBuffer(), buffer, combinedDataBuffer.size());
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
		const DeviceImpl& a_device,
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
		const DeviceImpl& a_device,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	)
	{
		OYL_PROFILE_FUNCTION();

		auto commandPool = CommandPoolImpl(a_device, { .commandQueueFlags = CommandQueueFlagBits::Transfer });
		auto commandBuffer = CommandBufferImpl(a_device, { .commandPool = commandPool });

		auto& vkCommandBuffer = commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		vkCommandBuffer.copyBuffer(*a_srcBuffer, *a_dstBuffer, vk::BufferCopy(0, 0, a_size));
		vkCommandBuffer.end();

		auto& transferQueue = *a_device.GetCommandQueue(CommandQueueFlagBits::Transfer);

		transferQueue.Submit({ .commandBuffer = commandBuffer });
		transferQueue.WaitUntilIdle();
	}
}
