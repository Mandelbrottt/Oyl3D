#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/StagingBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;

	using StagingBufferHandle = OpaqueHandleConvertible<StagingBufferHandle, vk::Buffer>;

	class OYL_RENDERING_API StagingBuffer : public Rendering::StagingBuffer
	{
	public:
		StagingBuffer();

		struct CreateParams
		{
			const Device& device;

			uint32 size;
			vk::BufferUsageFlags vkUsage;
			vk::MemoryPropertyFlags vkProperties;
		};

		explicit
		StagingBuffer(const CreateParams& a_params);

		StagingBuffer(StagingBuffer&& a_other) noexcept;
		StagingBuffer&
		operator =(StagingBuffer&& a_other) noexcept;

		virtual
		~StagingBuffer();

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

		StagingBufferHandle
		GetHandle() const;

		operator StagingBufferHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::StagingBufferHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
