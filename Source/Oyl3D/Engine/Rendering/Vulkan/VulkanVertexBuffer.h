#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class VulkanDevice;
	class VulkanCommandQueue;

	using VulkanVertexBufferId = OpaqueHandleConvertible<VertexBufferId, vk::Buffer>;

	class OYL_RENDERING_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(nullptr_t);

		VulkanVertexBuffer(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanVertexBuffer(VulkanVertexBuffer&& a_other) noexcept;
		VulkanVertexBuffer&
		operator =(VulkanVertexBuffer&& a_other) noexcept;

		virtual
		~VulkanVertexBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override;

		uint32
		GetVertexCount() const override;

		uint32
		GetVertexStride() const override;

		uint32
		GetVertexDataOffset() const;

		uint32
		GetIndexCount() const override;

		uint32
		GetIndexStride() const override;

		const vk::raii::Buffer&
		GetVkBuffer() const;

		VulkanVertexBufferId
		GetId() const;

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
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanVertexBufferHandle = PImpl<VulkanVertexBuffer>;
}
