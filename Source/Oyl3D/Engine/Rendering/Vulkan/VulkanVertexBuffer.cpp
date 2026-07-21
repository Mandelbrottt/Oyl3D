#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferResource::Impl
	{
		vk::raii::Buffer vertexBuffer = nullptr;
		vk::raii::DeviceMemory vertexBufferMemory = nullptr;

		void
		CreateVertexBuffer(
			const DeviceLoadParams& a_params,
			const byte* a_vertexData,
			size_t a_vertexLength,
			const byte* a_indexData,
			size_t a_indexLength
		);
	};

	VertexBufferResource::VertexBufferResource()
		: m_impl(std::make_unique<Impl>()) {}

	VertexBufferResource::VertexBufferResource(const VertexBufferOptions& a_options)
		: Rendering::VertexBufferResource(a_options),
		  m_impl(std::make_unique<Impl>()) {}

	VertexBufferResource::~VertexBufferResource() {}

	bool
	VertexBufferResource::Load()
	{
		return Rendering::VertexBufferResource::Load();
	}

	bool
	VertexBufferResource::Unload()
	{
		return Rendering::VertexBufferResource::Unload();
	}

	bool
	VertexBufferResource::DeviceLoad(void* a_params)
	{
		const auto& params = *static_cast<DeviceLoadParams*>(a_params);

		m_impl->CreateVertexBuffer(
			params,
			m_vertexData.data(),
			m_vertexData.size(),
			m_indexData.data(),
			m_indexData.size()
		);

		return Rendering::VertexBufferResource::DeviceLoad(a_params);
	}

	bool
	VertexBufferResource::DeviceUnload(void* a_params)
	{
		m_impl->vertexBufferMemory.release();
		m_impl->vertexBuffer.release();

		return Rendering::VertexBufferResource::DeviceUnload(a_params);
	}

	const vk::raii::Buffer&
	VertexBufferResource::GetVkBuffer()
	{
		return m_impl->vertexBuffer;
	}

	static
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
		const VertexBufferResource::DeviceLoadParams& a_params,
		vk::DeviceSize a_size,
		vk::BufferUsageFlags a_usage,
		vk::MemoryPropertyFlags a_properties
	)
	{
		vk::BufferCreateInfo bufferInfo { .size = a_size, .usage = a_usage, .sharingMode = vk::SharingMode::eExclusive };
		vk::raii::Buffer buffer = vk::raii::Buffer(a_params.device, bufferInfo);
		vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo { .allocationSize = memRequirements.size, .memoryTypeIndex = FindMemoryType(a_params.physicalDevice, memRequirements.memoryTypeBits, a_properties) };
		vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(a_params.device, allocInfo);
		buffer.bindMemory(*bufferMemory, 0);
		return { std::move(buffer), std::move(bufferMemory) };
	}

	void
	CopyBuffer(
		const VertexBufferResource::DeviceLoadParams& a_params,
		const vk::raii::Buffer& a_srcBuffer,
		const vk::raii::Buffer& a_dstBuffer,
		vk::DeviceSize a_size
	)
	{
		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = a_params.commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		vk::raii::CommandBuffer commandCopyBuffer =
			std::move(a_params.device.allocateCommandBuffers(allocInfo).front());

		commandCopyBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		commandCopyBuffer.copyBuffer(*a_srcBuffer, *a_dstBuffer, vk::BufferCopy(0, 0, a_size));
		commandCopyBuffer.end();

		a_params.queue.submit(
			vk::SubmitInfo {
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandCopyBuffer
			},
			nullptr
		);
		a_params.queue.waitIdle();
	}

	void
	VertexBufferResource::Impl::CreateVertexBuffer(
		const DeviceLoadParams& a_params,
		const byte* a_vertexData,
		size_t a_vertexLength,
		const byte* a_indexData,
		size_t a_indexLength
	)
	{
		// Combine vertex and index data into one contiguous buffer
		std::vector<byte> combinedDataBuffer;
		combinedDataBuffer.reserve(a_vertexLength + a_indexLength);
		if (a_indexLength > 0)
			combinedDataBuffer.insert(combinedDataBuffer.end(), &a_indexData[0], &a_indexData[a_indexLength]);
		combinedDataBuffer.insert(combinedDataBuffer.end(), &a_vertexData[0], &a_vertexData[a_vertexLength]);

		// Create staging buffer to send data from CPU to GPU
		auto [stagingBuffer, stagingBufferMemory] =
			CreateBuffer(
				a_params,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible
				| vk::MemoryPropertyFlagBits::eHostCoherent
			);

		void* dataStaging = stagingBufferMemory.mapMemory(0, combinedDataBuffer.size());
		std::memcpy(dataStaging, combinedDataBuffer.data(), combinedDataBuffer.size());
		stagingBufferMemory.unmapMemory();

		// Copy data in staging buffer to main buffer
		std::tie(vertexBuffer, vertexBufferMemory) =
			CreateBuffer(
				a_params,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eIndexBuffer
				| vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

		CopyBuffer(a_params, stagingBuffer, vertexBuffer, combinedDataBuffer.size());
	}
}
