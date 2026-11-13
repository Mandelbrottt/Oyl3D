#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/Hardware/RdoUniformBuffer.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanUniformBufferId = IConvertibleRenderDeviceObjectId<RdoUniformBufferId, vk::Buffer>;

	class RENDERING_VULKAN_EXPORT VulkanUniformBuffer : public IRdoUniformBuffer
	{
	public:
		VulkanUniformBuffer(nullptr_t) {}

		explicit
		VulkanUniformBuffer(const VulkanDevice& a_device, const RdoUniformBufferCreateParams& a_params);

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
		RdoUniformBufferId
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
