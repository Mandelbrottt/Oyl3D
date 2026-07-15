#include "VulkanVertexBuffer.h"

#include <vulkan/vulkan_raii.hpp>

namespace Oyl::Rendering::Vulkan
{
	struct VertexBufferResource::Impl
	{
		vk::raii::Buffer vertexBuffer = nullptr;
		vk::raii::DeviceMemory vertexBufferMemory = nullptr;

		void
		CreateVertexBuffer(const VertexBufferParams& a_params, const byte* a_data, size_t a_dataLength);
		void
		AllocateBufferMemory(const VertexBufferParams& a_params, const byte* a_data, size_t a_dataLength);
	};

	VertexBufferResource::VertexBufferResource()
		: m_impl(std::make_unique<Impl>()) {}

	VertexBufferResource::VertexBufferResource(const byte* a_data, size_t a_size)
		: Rendering::VertexBufferResource(a_data, a_size),
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
		const auto& params = *static_cast<VertexBufferParams*>(a_params);

		m_impl->CreateVertexBuffer(params, m_data.data(), m_data.size());

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
	VertexBufferResource::GetVertexBuffer()
	{
		return m_impl->vertexBuffer;
	}

	void
	VertexBufferResource::Impl::CreateVertexBuffer(
		const VertexBufferParams& a_params,
		const byte* a_data,
		size_t a_dataLength
	)
	{
		vk::BufferCreateInfo bufferInfo {
			.size = a_dataLength,
			.usage = vk::BufferUsageFlagBits::eVertexBuffer,
			.sharingMode = vk::SharingMode::eExclusive
		};
		vertexBuffer = vk::raii::Buffer(a_params.device, bufferInfo);

		AllocateBufferMemory(a_params, a_data, a_dataLength);
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

	void
	VertexBufferResource::Impl::AllocateBufferMemory(
		const VertexBufferParams& a_params,
		const byte* a_data,
		size_t a_dataLength
	)
	{
		OYL_ASSERT(vertexBuffer != nullptr);

		auto memoryRequirements = vertexBuffer.getMemoryRequirements();

		vk::MemoryAllocateInfo memoryAllocateInfo {
			.allocationSize = memoryRequirements.size,
			.memoryTypeIndex = FindMemoryType(
				a_params.physicalDevice,
				memoryRequirements.memoryTypeBits,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			)
		};
		vertexBufferMemory = vk::raii::DeviceMemory(a_params.device, memoryAllocateInfo);
		OYL_ASSERT(vertexBufferMemory != nullptr);
		vertexBuffer.bindMemory(*vertexBufferMemory, 0);

		void* vertexBufferData = vertexBufferMemory.mapMemory(0, a_dataLength);
		std::memcpy(vertexBufferData, a_data, a_dataLength);
		vertexBufferMemory.unmapMemory();
	}
}
