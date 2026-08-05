#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/StagingBuffer.h"

namespace Oyl::Rendering
{
	class VulkanDevice;

	using VulkanStagingBufferId = OpaqueHandleConvertible<StagingBufferId, vk::Buffer>;

	class OYL_RENDERING_API VulkanStagingBuffer : public StagingBuffer
	{
	public:
		VulkanStagingBuffer();

		struct CreateParams
		{
			const byte* pData = nullptr;

			uint32 dataLength;
			vk::BufferUsageFlags vkUsage;
			vk::MemoryPropertyFlags vkProperties;
		};

		explicit
		VulkanStagingBuffer(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanStagingBuffer(VulkanStagingBuffer&& a_other) noexcept;
		VulkanStagingBuffer&
		operator =(VulkanStagingBuffer&& a_other) noexcept;

		virtual
		~VulkanStagingBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override;

		bool
		CopyMemory(const void* a_src, uint32 a_size) override;

		const vk::raii::Buffer&
		GetVkBuffer() const;

		const vk::raii::DeviceMemory&
		GetVkDeviceMemory() const;

		VulkanStagingBufferId
		GetId() const;

		operator VulkanStagingBufferId() const
		{
			return GetId();
		}

	protected:
		StagingBufferId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanStagingBufferHandle = PImpl<VulkanStagingBuffer>;
}
