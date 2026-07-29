#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class Device;
	class RenderQueue;

	using VertexBufferHandle = OpaqueHandleConvertible<VertexBufferHandle, vk::Buffer>;

	class OYL_RENDERING_API VertexBuffer : public Rendering::VertexBuffer
	{
	public:
		VertexBuffer();

		struct CreateParams
		{
			const Device& device;
			const RenderQueue& queue;

			const byte* vertexData;
			size_t vertexLength;
			uint32 vertexStride;

			const byte* indexData = nullptr;
			size_t indexLength = 0;
			size_t indexStride = sizeof(uint16);
		};

		explicit
		VertexBuffer(const CreateParams& a_params);

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
		GetHandle() const
		{
			return GetHandleImpl();
		}

	protected:
		Rendering::VertexBufferHandle
		GetHandleImpl() const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
