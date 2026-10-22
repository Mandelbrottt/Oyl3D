#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/VertexBuffer.h"

namespace Oyl
{
	class VulkanDevice;

	using VulkanVertexBufferId = IConvertibleDeviceObjectId<VertexBufferId, vk::Buffer>;

	class VulkanVertexBuffer : public IVertexBuffer
	{
	public:
		VulkanVertexBuffer(nullptr_t) {}

		explicit
		VulkanVertexBuffer(const VulkanDevice& a_device, const VertexBufferCreateParams& a_params);

		DEFAULT_MOVE(VulkanVertexBuffer);

		virtual
		~VulkanVertexBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_buffer; }

		uint32
		GetVertexCount() const override { return m_vertexCount; }

		uint32
		GetVertexStride() const override { return m_vertexStride; }

		uint32
		GetVertexDataOffset() const
		{
			if (m_indexCount == 0)
				return 0;
			return m_indexCount * m_indexStride;
		}

		uint32
		GetIndexCount() const override { return m_indexCount; }

		uint32
		GetIndexStride() const override { return m_indexStride; }

		const vk::raii::Buffer&
		GetVkBuffer() const { return m_buffer; }

		VulkanVertexBufferId
		GetId() const { return *m_buffer; }

		operator VulkanVertexBufferId() const
		{
			return GetId();
		}

	protected:
		VertexBufferId
		GetIdImpl() const override
		{
			return GetId();
		}

	private:
		void
		CreateVertexBuffer(const VulkanDevice& a_device, const VertexBufferCreateParams& a_params);

	private:
		vk::raii::Buffer m_buffer = nullptr;
		vk::raii::DeviceMemory m_bufferMemory = nullptr;

		uint32 m_vertexCount = 0;
		uint32 m_vertexStride = 0;

		uint32 m_indexCount = 0;
		uint32 m_indexStride = 0;
	};
}
