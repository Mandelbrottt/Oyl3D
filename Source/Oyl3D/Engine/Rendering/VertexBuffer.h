#pragma once

#include <Core/Array.h>
#include <Core/PImpl.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct VertexBufferHandle : OpaqueHandle<VertexBufferHandle> {};

	class VertexBufferImpl : public IDeviceObject<VertexBufferHandle>
	{
	protected:
		VertexBufferImpl() = default;

	public:
		struct CreateParams
		{
			ArrayProxy<byte> vertexData;
			uint32 vertexStride;

			ArrayProxy<byte> indexData;
			uint32 indexStride = sizeof(uint16);
		};

		NO_MOVE(VertexBufferImpl);
		NO_COPY(VertexBufferImpl);

		virtual
		~VertexBufferImpl() = default;

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

	using VertexBuffer = PImpl<VertexBufferImpl>;
}
