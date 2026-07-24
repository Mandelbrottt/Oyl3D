#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferResource::Impl
	{
		vk::raii::Buffer vertexBuffer = nullptr;
		vk::raii::DeviceMemory vertexBufferMemory = nullptr;

		void
		CreateVertexBuffer(
			const Device& a_params,
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
	VertexBufferResource::DeviceLoad(const Rendering::Device& a_device)
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = static_cast<const Device&>(a_device);

		m_impl->CreateVertexBuffer(
			device,
			m_vertexData.data(),
			m_vertexData.size(),
			m_indexData.data(),
			m_indexData.size()
		);

		return Rendering::VertexBufferResource::DeviceLoad(a_device);
	}

	bool
	VertexBufferResource::DeviceUnload()
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vertexBufferMemory.release();
		m_impl->vertexBuffer.release();

		return Rendering::VertexBufferResource::DeviceUnload();
	}

	const vk::raii::Buffer&
	VertexBufferResource::GetVkBuffer() const
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
				.queueFamilyIndex = a_device.GetVkGraphicsQueueIndex()
			}
		);

		const auto& vkDevice = a_device.GetVkDevice();
		const auto& vkQueue = a_device.GetVkGraphicsQueue();

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

		vkQueue.submit(
			vk::SubmitInfo {
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandCopyBuffer
			},
			nullptr
		);
		vkQueue.waitIdle();
	}

	void
	VertexBufferResource::Impl::CreateVertexBuffer(
		const Device& a_device,
		const byte* a_vertexData,
		size_t a_vertexLength,
		const byte* a_indexData,
		size_t a_indexLength
	)
	{
		OYL_PROFILE_FUNCTION();

		// Combine vertex and index data into one contiguous buffer
		std::vector<byte> combinedDataBuffer;
		combinedDataBuffer.reserve(a_vertexLength + a_indexLength);
		if (a_indexLength > 0)
			combinedDataBuffer.insert(combinedDataBuffer.end(), &a_indexData[0], &a_indexData[a_indexLength]);
		combinedDataBuffer.insert(combinedDataBuffer.end(), &a_vertexData[0], &a_vertexData[a_vertexLength]);

		// Create staging buffer to send data from CPU to GPU
		auto [stagingBuffer, stagingBufferMemory] =
			CreateBuffer(
				a_device,
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
				a_device,
				combinedDataBuffer.size(),
				vk::BufferUsageFlagBits::eVertexBuffer
				| vk::BufferUsageFlagBits::eIndexBuffer
				| vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);

		CopyBuffer(a_device, stagingBuffer, vertexBuffer, combinedDataBuffer.size());
	}
}
