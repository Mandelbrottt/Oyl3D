#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanRenderQueue.h"

namespace Oyl::Rendering::Vulkan
{
	struct VertexBuffer::Impl
	{
		vk::raii::Buffer buffer = nullptr;
		vk::raii::DeviceMemory bufferMemory = nullptr;

		uint32 vertexCount;
		uint32 vertexStride;

		uint32 indexCount = 0;
		uint32 indexStride = 0;

		void
		CreateVertexBuffer(const CreateParams& a_params);
	};

	VertexBuffer::VertexBuffer()
		: m_impl(nullptr) {}

	VertexBuffer::VertexBuffer(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_params.vertexLength % a_params.vertexStride == 0);
		m_impl->vertexCount = a_params.vertexLength / a_params.vertexStride;
		m_impl->vertexStride = a_params.vertexStride;

		if (a_params.indexData && a_params.indexLength != 0 && a_params.indexStride != 0)
		{
			OYL_ASSERT(a_params.indexLength % a_params.indexStride == 0);
			m_impl->indexCount = a_params.indexLength / a_params.indexStride;
			m_impl->indexStride = a_params.indexStride;
		}

		m_impl->CreateVertexBuffer(a_params);
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VertexBuffer&
	VertexBuffer::operator=(VertexBuffer&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	VertexBuffer::~VertexBuffer()
	{
		VertexBuffer::Destroy();
	}

	void
	VertexBuffer::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->buffer.clear();
	}

	bool
	VertexBuffer::IsValid() const
	{
		return m_impl
		       && *m_impl->buffer;
	}

	uint32
	VertexBuffer::GetVertexCount() const
	{
		return m_impl->vertexCount;
	}

	uint32
	VertexBuffer::GetVertexStride() const
	{
		return m_impl->vertexStride;
	}

	uint32
	VertexBuffer::GetVertexDataOffset() const
	{
		if (m_impl->indexCount == 0)
			return 0;

		return m_impl->indexCount * m_impl->indexStride;
	}

	uint32
	VertexBuffer::GetIndexCount() const
	{
		return m_impl->indexCount;
	}

	uint32
	VertexBuffer::GetIndexStride() const
	{
		return m_impl->indexStride;
	}

	const vk::raii::Buffer&
	VertexBuffer::GetVkBuffer() const
	{
		return m_impl->buffer;
	}

	VertexBufferHandle
	VertexBuffer::GetHandle() const
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
		const Device& a_device,
		vk::DeviceSize a_size,
		vk::BufferUsageFlags a_usage,
		vk::MemoryPropertyFlags a_properties
	);

	static
	void
	CopyBuffer(
		const Device& a_device,
		const RenderQueue& a_queue,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	);

	void
	VertexBuffer::Impl::CreateVertexBuffer(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = a_params.device;
		const auto& queue = a_params.queue;

		auto vertexData = a_params.vertexData;
		auto vertexLength = a_params.vertexLength;
		auto indexData = a_params.indexData;
		auto indexLength = a_params.indexLength;

		// Combine vertex and index data into one contiguous buffer
		std::vector<byte> combinedDataBuffer;
		combinedDataBuffer.reserve(vertexLength + indexLength);
		if (indexLength > 0)
			combinedDataBuffer.insert(combinedDataBuffer.end(), &indexData[0], &indexData[indexLength]);
		combinedDataBuffer.insert(combinedDataBuffer.end(), &vertexData[0], &vertexData[vertexLength]);

		// Create staging buffer to send data from CPU to GPU
		auto [stagingBuffer, stagingBufferMemory] =
			CreateBuffer(
				device,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible
				| vk::MemoryPropertyFlagBits::eHostCoherent
			);

		void* dataStaging = stagingBufferMemory.mapMemory(0, combinedDataBuffer.size());
		std::memcpy(dataStaging, combinedDataBuffer.data(), combinedDataBuffer.size());
		stagingBufferMemory.unmapMemory();

		// Copy data in staging buffer to main buffer
		std::tie(buffer, bufferMemory) =
			CreateBuffer(
				device,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eIndexBuffer
				| vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

		CopyBuffer(device, queue, stagingBuffer, buffer, combinedDataBuffer.size());
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
		const Device& a_device,
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
		const Device& a_device,
		const RenderQueue& a_queue,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	)
	{
		OYL_PROFILE_FUNCTION();

		auto commandPool = vk::raii::CommandPool(
			a_device.GetVkDevice(),
			{
				.flags = vk::CommandPoolCreateFlagBits::eTransient,
				.queueFamilyIndex = a_device.GetVkGraphicsQueueFamilyIndex()
			}
		);

		const auto& vkDevice = a_device.GetVkDevice();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		vk::raii::CommandBuffer commandCopyBuffer =
			std::move(vkDevice.allocateCommandBuffers(allocInfo).front());

		commandCopyBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		commandCopyBuffer.copyBuffer(*a_srcBuffer, *a_dstBuffer, vk::BufferCopy(0, 0, a_size));
		commandCopyBuffer.end();

		const auto& vkQueue = a_queue.GetVkQueue();

		vkQueue.submit(
			vk::SubmitInfo {
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandCopyBuffer
			},
			nullptr
		);
		vkQueue.waitIdle();
	}
}
