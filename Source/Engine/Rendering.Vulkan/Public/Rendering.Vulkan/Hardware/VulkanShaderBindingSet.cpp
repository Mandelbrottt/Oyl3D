#include "VulkanShaderBindingSet.h"

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanShaderBindingSet::VulkanShaderBindingSet(
		const VulkanDevice& a_device,
		const RdoShaderBindingSetCreateParams& a_params
	)
	{
		OYL_PROFILE_FUNCTION();

		m_stride = 0;
		for (const auto& binding : a_params.bindings)
			m_stride += binding.stride;

		OYL_ASSERT(m_stride != 0);

		CreateDescriptorSets(a_device, a_params.bindings);
		CreateBindingBuffer(a_device);
		UpdateDescriptorSets(a_device);
	}

	VulkanShaderBindingSet::~VulkanShaderBindingSet()
	{
		VulkanShaderBindingSet::Destroy();
	}

	void
	VulkanShaderBindingSet::Destroy()
	{
		if (!IsValid())
			return;

		m_vkBuffer.clear();
		m_vkBufferMemory.clear();
		m_mappedMemory = nullptr;
	}

	void
	VulkanShaderBindingSet::CopyToBuffer(const void* a_data, uint64 a_size, uint64 a_offset)
	{
		void* pDest = (void*) ((uintptr_t) m_mappedMemory + a_offset);

		void* pEnd = (void*) ((uintptr_t) m_mappedMemory + m_stride);
		void* pCopyEnd = (void*) ((uintptr_t) pDest + a_size);
		if (pEnd < pCopyEnd)
		{
			LogError(
				"Trying to copy {{{}}} with size \"{}\" to buffer {{{}}} with size \"{}\" at offset \"{}\""
				", would cause buffer overrun!",
				a_data,
				a_size,
				std::bit_cast<uint64>(GetId()),
				m_stride,
				a_offset);
			return;
		}

		std::memcpy(pDest, a_data, a_size);
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
	VulkanShaderBindingSet::CreateDescriptorSets(const VulkanDevice& a_device, const Array<ShaderBufferBinding>& a_bindings)
	{
		m_bufferBindings = a_bindings;

		// Recreate layout
		Array<vk::DescriptorSetLayoutBinding> uboBindings;
		for (const auto& bufferBinding : m_bufferBindings)
		{
			vk::DescriptorSetLayoutBinding uboLayoutBinding {
				.binding = bufferBinding.binding,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.descriptorCount = 1,
			};
			if (bufferBinding.stage == ShaderProfileFlagBits::Vertex)
				uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
			if (bufferBinding.stage == ShaderProfileFlagBits::Fragment)
				uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
			uboBindings.Add(uboLayoutBinding);
		}
		m_vkDescriptorSetLayout = vk::raii::DescriptorSetLayout(
			a_device.GetVkDevice(),
			{
				.bindingCount = uboBindings.Length(),
				.pBindings = uboBindings.Data(),
			});

		// Create DescriptorPool
		vk::DescriptorPoolSize poolSize {
			.type = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = m_bufferBindings.Length(),
		};
		vk::DescriptorPoolCreateInfo poolInfo = {
			.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
			.maxSets = poolSize.descriptorCount,
			.poolSizeCount = 1,
			.pPoolSizes = &poolSize,
		};
		m_vkDescriptorPool = vk::raii::DescriptorPool(a_device.GetVkDevice(), poolInfo);

		// Allocate Descriptor Sets
		vk::DescriptorSetAllocateInfo allocInfo {
			.descriptorPool = m_vkDescriptorPool,
			.descriptorSetCount = m_bufferBindings.Length(),
			.pSetLayouts = &*m_vkDescriptorSetLayout,
		};
		m_vkDescriptorSets = vk::raii::DescriptorSets(a_device.GetVkDevice(), allocInfo);
	}

	void
	VulkanShaderBindingSet::CreateBindingBuffer(const VulkanDevice& a_device)
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

	void
	VulkanShaderBindingSet::UpdateDescriptorSets(const VulkanDevice& a_device)
	{
		Array<vk::DescriptorBufferInfo> bufferInfos;
		bufferInfos.reserve(m_bufferBindings.Length());
		Array<vk::WriteDescriptorSet> writeDescriptorSets;
		writeDescriptorSets.reserve(bufferInfos.Length());

		uint offset = 0;
		for (uint i = 0; i < m_vkDescriptorSets.size(); i++)
		{
			auto& vkDescriptorSet = m_vkDescriptorSets[i];
			auto& bufferBinding = m_bufferBindings[i];

			bufferInfos.Add({
				.buffer = m_vkBuffer,
				.offset = offset,
				.range = bufferBinding.stride
			});
			offset += bufferBinding.stride;
			writeDescriptorSets.Add({
				.dstSet = *vkDescriptorSet,
				.dstBinding = bufferBinding.binding,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.pBufferInfo = &bufferInfos.back(),
			});
		}
		a_device.GetVkDevice().updateDescriptorSets(writeDescriptorSets, {});
	}
}
