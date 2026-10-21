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

	class IVertexBuffer : public IDeviceObject<VertexBufferId>
	{
	protected:
		IVertexBuffer() = default;
		DEFAULT_MOVE(IVertexBuffer);

	public:
		NO_COPY(IVertexBuffer);

		virtual
		~IVertexBuffer() = 0;

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
	IVertexBuffer::~IVertexBuffer() {}

	using VertexBufferHandle = IObjectHandle<IVertexBuffer>;
}
