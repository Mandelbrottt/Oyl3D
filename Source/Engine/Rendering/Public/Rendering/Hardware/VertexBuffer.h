#pragma once

#include <Core/Containers/Array.h>

#include "RenderDeviceObject.h"

namespace Oyl
{
	struct RdoVertexBufferId : IRenderDeviceObjectId<RdoVertexBufferId> {};

	struct RdoVertexBufferCreateParams
	{
		ArrayProxy<byte> vertexData;
		uint32 vertexStride;

		ArrayProxy<byte> indexData;
		uint32 indexStride = sizeof(uint16);
	};

	class IRdoVertexBuffer : public IRenderDeviceObject<RdoVertexBufferId>
	{
	protected:
		IRdoVertexBuffer() = default;
		DEFAULT_MOVE(IRdoVertexBuffer);

	public:
		NO_COPY(IRdoVertexBuffer);

		virtual
		~IRdoVertexBuffer() = 0;

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
	IRdoVertexBuffer::~IRdoVertexBuffer() {}

	using RdoVertexBufferHandle = IRenderDeviceObjectHandle<IRdoVertexBuffer>;
}
