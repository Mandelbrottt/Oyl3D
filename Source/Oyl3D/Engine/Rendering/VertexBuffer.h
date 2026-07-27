#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct VertexBufferHandle : OpaqueHandle<VertexBufferHandle> {};

	class VertexBuffer : public DeviceObject<VertexBufferHandle>
	{
	protected:
		VertexBuffer() = default;

		DEFAULT_MOVE(VertexBuffer);

	public:
		NO_COPY(VertexBuffer);

		virtual
		~VertexBuffer() = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		uint32
		GetVertexCount() const = 0;

		virtual
		uint32
		GetVertexStride() const = 0;

		virtual
		uint32
		GetIndexCount() const = 0;

		virtual
		uint32
		GetIndexStride() const = 0;
	};
}
