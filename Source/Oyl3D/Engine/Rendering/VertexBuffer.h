#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct VertexBufferHandle : OpaqueHandle<VertexBufferHandle> {};

	class VertexBuffer : public IDeviceObject<VertexBufferHandle>
	{
	protected:
		VertexBuffer() = default;

		DEFAULT_MOVE(VertexBuffer);

	public:
		NO_COPY(VertexBuffer);

		virtual
		~VertexBuffer() = default;

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
