#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;
	class CommandQueue;

	using VertexBufferHandle = OpaqueHandleConvertible<VertexBufferHandle, vk::Buffer>;

	class OYL_RENDERING_API VertexBuffer : public Rendering::VertexBuffer
	{
	public:
		VertexBuffer();

		struct CreateParams
		{
			const byte* vertexData;
			uint32 vertexLength;
			uint32 vertexStride;

			const byte* indexData = nullptr;
			uint32 indexLength = 0;
			uint32 indexStride = sizeof(uint16);
		};

		explicit
		VertexBuffer(const DeviceImpl& a_device, const CreateParams& a_params);

		VertexBuffer(VertexBuffer&& a_other) noexcept;
		VertexBuffer&
		operator =(VertexBuffer&& a_other) noexcept;

		virtual
		~VertexBuffer();

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
}
