#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Hardware/RdoShaderBindingSet.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanShaderBindingSetId = IConvertibleRenderDeviceObjectId<RdoShaderBindingSetId, vk::Buffer>;

	class RENDERING_VULKAN_EXPORT VulkanShaderBindingSet : public IRdoShaderBindingSet
	{
	public:
		VulkanShaderBindingSet(nullptr_t) {}

		explicit
		VulkanShaderBindingSet(const VulkanDevice& a_device, const RdoShaderBindingSetCreateParams& a_params);

		DEFAULT_MOVE(VulkanShaderBindingSet);

		virtual
		~VulkanShaderBindingSet();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkBuffer; }

		const Array<ShaderBufferBinding>&
		GetBufferBindings() const override { return m_bufferBindings; }

		void*
		GetUnderlyingBuffer() override { return m_mappedMemory; }

		void
		CopyToBuffer(const void* a_data, uint64 a_size, uint64 a_offset = 0) override;

		const vk::raii::Buffer&
		GetVkBuffer() const { return m_vkBuffer; }

		const vk::raii::DescriptorSetLayout&
		GetVkDescriptorSetLayout() const { return m_vkDescriptorSetLayout; }

		const vk::raii::DescriptorSets&
		GetVkDescriptorSets() const { return m_vkDescriptorSets; }

		VulkanShaderBindingSetId
		GetId() const { return *m_vkBuffer; }

		operator VulkanShaderBindingSetId() const
		{
			return GetId();
		}

	protected:
		RdoShaderBindingSetId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		void
		CreateDescriptorSets(const VulkanDevice& a_device, const Array<ShaderBufferBinding>& a_bindings);

		void
		CreateBindingBuffer(const VulkanDevice& a_device);

		void
		UpdateDescriptorSets(const VulkanDevice& a_device);

	private:
		vk::raii::Buffer m_vkBuffer = nullptr;
		vk::raii::DeviceMemory m_vkBufferMemory = nullptr;
		void* m_mappedMemory = nullptr;
		uint64 m_stride = 0;

		Array<ShaderBufferBinding> m_bufferBindings;

		vk::raii::DescriptorSetLayout m_vkDescriptorSetLayout = nullptr;
		vk::raii::DescriptorPool m_vkDescriptorPool = nullptr;
		vk::raii::DescriptorSets m_vkDescriptorSets = nullptr;
	};
}
