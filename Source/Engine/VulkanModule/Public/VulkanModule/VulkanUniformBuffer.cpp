#include "VulkanUniformBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanUniformBuffer::VulkanUniformBuffer(const VulkanDevice& a_device, const UniformBufferCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(a_params.stride != 0);
		m_stride = a_params.stride;

		CreateUniformBuffer(a_device);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VulkanUniformBuffer::Destroy();
	}

	void
	VulkanUniformBuffer::Destroy()
	{
		if (!IsValid())
			return;

		m_vkBuffer.clear();
	}

	void
	VulkanUniformBuffer::CopyToBuffer(void* a_data, uint64 a_size)
	{
		if (a_size != m_stride)
		{
			LogError("Trying to copy {{{}}} with size \"{}\" to buffer {{{}}} with size \"{}\"!", a_data, a_size, std::bit_cast<uint64>(GetId()), m_stride);
			return;
		}

		std::memcpy(m_mappedMemory, a_data, a_size);
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

	void
	VulkanUniformBuffer::CreateUniformBuffer(const VulkanDevice& a_device)
	{
		OYL_PROFILE_FUNCTION();

		// Copy data in staging buffer to main buffer
		std::tie(m_vkBuffer, m_vkBufferMemory) =
			CreateBuffer(
				a_device,
				m_stride,
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible
				| vk::MemoryPropertyFlagBits::eHostCoherent
			);
		m_mappedMemory = m_vkBufferMemory.mapMemory(0, m_stride);
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
}
