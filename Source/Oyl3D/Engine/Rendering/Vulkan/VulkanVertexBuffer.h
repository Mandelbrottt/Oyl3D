#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;
	class CommandQueueImpl;

	using VertexBufferHandle = OpaqueHandleConvertible<VertexBufferHandle, vk::Buffer>;

	class OYL_RENDERING_API VertexBufferImpl : public Rendering::VertexBufferImpl
	{
	public:
		VertexBufferImpl(nullptr_t);

		VertexBufferImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		VertexBufferImpl(VertexBufferImpl&& a_other) noexcept;
		VertexBufferImpl&
		operator =(VertexBufferImpl&& a_other) noexcept;

		virtual
		~VertexBufferImpl();

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

		VertexBufferHandle
		GetHandle() const;

		operator VertexBufferHandle() const
		{
			return GetHandle();
		}

	protected:
		Rendering::VertexBufferHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VertexBuffer = PImpl<VertexBufferImpl>;
}
