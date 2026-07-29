#include "VulkanStagingBuffer.h"

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct StagingBuffer::Impl
	{
		vk::raii::Buffer vkBuffer = nullptr;
		vk::raii::DeviceMemory vkBufferMemory = nullptr;

		uint32 size;

		void
		CreateBuffer(const Device& a_device, const CreateParams& a_params);
	};

	StagingBuffer::StagingBuffer()
		: m_impl(nullptr) {}

	StagingBuffer::StagingBuffer(const Device& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->CreateBuffer(a_device, a_params);

		if (a_params.pData)
			StagingBuffer::CopyMemory(a_params.pData, m_impl->size);
	}

	StagingBuffer::StagingBuffer(StagingBuffer&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	StagingBuffer&
	StagingBuffer::operator=(StagingBuffer&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	StagingBuffer::~StagingBuffer()
	{
		StagingBuffer::Destroy();
	}

	void
	StagingBuffer::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->vkBufferMemory.clear();
		m_impl->vkBuffer.clear();
	}

	bool
	StagingBuffer::IsValid() const
	{
		return m_impl
		       && *m_impl->vkBuffer
		       && *m_impl->vkBufferMemory;
	}

	bool
	StagingBuffer::CopyMemory(const void* a_src, uint32 a_size)
	{
		OYL_PROFILE_FUNCTION();

		if (m_impl->size != a_size)
			return false;

		void* copyDst = m_impl->vkBufferMemory.mapMemory(0, a_size);
		std::memcpy(copyDst, a_src, a_size);
		m_impl->vkBufferMemory.unmapMemory();
		return true;
	}

	const vk::raii::Buffer&
	StagingBuffer::GetVkBuffer() const
	{
		return m_impl->vkBuffer;
	}

	const vk::raii::DeviceMemory&
	StagingBuffer::GetVkDeviceMemory() const
	{
		return m_impl->vkBufferMemory;
	}

	StagingBufferHandle
	StagingBuffer::GetHandle() const
	{
		return *m_impl->vkBuffer;
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
	StagingBuffer::Impl::CreateBuffer(const Device& a_device, const CreateParams& a_params)
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
		vkBuffer = vk::raii::Buffer(device, bufferInfo);
		size = a_params.dataLength;

		vk::MemoryRequirements memRequirements = vkBuffer.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo {
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, a_params.vkProperties)
		};
		vkBufferMemory = vk::raii::DeviceMemory(device, allocInfo);
		vkBuffer.bindMemory(*vkBufferMemory, 0);
	}
}
