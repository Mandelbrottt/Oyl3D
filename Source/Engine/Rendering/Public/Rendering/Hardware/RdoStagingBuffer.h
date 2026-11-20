#pragma once

#include <Core/Types/Primitives.h>

#include "RenderDeviceObject.h"

namespace Oyl
{
	struct RdoStagingBufferId : IRenderDeviceObjectId<RdoStagingBufferId> {};

	class IRdoStagingBuffer : public IRenderDeviceObject<RdoStagingBufferId>
	{
	protected:
		IRdoStagingBuffer() = default;

		DEFAULT_MOVE(IRdoStagingBuffer);

	public:
		NO_COPY(IRdoStagingBuffer);

		virtual
		~IRdoStagingBuffer() = 0;

		virtual
		bool
		CopyMemory(const void* a_src, uint32 a_size) = 0;
	};

	inline
	IRdoStagingBuffer::~IRdoStagingBuffer() {}

	using RdoStagingBufferHandle = IRenderDeviceObjectHandle<IRdoStagingBuffer>;
}
