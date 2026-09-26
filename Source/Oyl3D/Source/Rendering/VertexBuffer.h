#pragma once

#include <Core/Array.h>
#include <Core/PImpl.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct VertexBufferId : OpaqueHandle<VertexBufferId> {};

	class VertexBuffer : public IDeviceObject<VertexBufferId>
	{
	protected:
		VertexBuffer() = default;

	public:
		struct CreateParams
		{
			ArrayProxy<byte> vertexData;
			uint32 vertexStride;

			ArrayProxy<byte> indexData;
			uint32 indexStride = sizeof(uint16);
		};

		NO_MOVE(VertexBuffer);
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

	using VertexBufferHandle = PImpl<VertexBuffer>;
}
