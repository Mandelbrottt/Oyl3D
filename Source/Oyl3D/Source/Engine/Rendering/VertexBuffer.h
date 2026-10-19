#pragma once

#include <Core/Containers/Array.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct VertexBufferId : IDeviceObjectId<VertexBufferId> {};

	struct VertexBufferCreateParams
	{
		ArrayProxy<byte> vertexData;
		uint32 vertexStride;

		ArrayProxy<byte> indexData;
		uint32 indexStride = sizeof(uint16);
	};

	class VertexBuffer : public IDeviceObject<VertexBufferId>
	{
	protected:
		VertexBuffer() = default;
		DEFAULT_MOVE(VertexBuffer);

	public:
		NO_COPY(VertexBuffer);

		virtual
		~VertexBuffer() = 0;

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

	inline
	VertexBuffer::~VertexBuffer() {}
}
