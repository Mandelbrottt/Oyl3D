#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/UniformBuffer.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanUniformBufferId = IConvertibleDeviceObjectId<UniformBufferId, vk::Buffer>;

	class VULKANMODULE_EXPORT VulkanUniformBuffer : public IUniformBuffer
	{
	public:
		VulkanUniformBuffer(nullptr_t) {}

		explicit
		VulkanUniformBuffer(const VulkanDevice& a_device, const UniformBufferCreateParams& a_params);

		DEFAULT_MOVE(VulkanUniformBuffer);

		virtual
		~VulkanUniformBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkBuffer; }

		void
		CopyToBuffer(void* a_data, uint64 a_size) override;

		const vk::raii::Buffer&
		GetVkBuffer() const { return m_vkBuffer; }

		VulkanUniformBufferId
		GetId() const { return *m_vkBuffer; }

		operator VulkanUniformBufferId() const
		{
			return GetId();
		}

	protected:
		UniformBufferId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		void
		CreateUniformBuffer(const VulkanDevice& a_device);

	private:
		vk::raii::Buffer m_vkBuffer = nullptr;
		vk::raii::DeviceMemory m_vkBufferMemory = nullptr;
		void* m_mappedMemory = nullptr;

		uint64 m_stride = 0;
	};
}
