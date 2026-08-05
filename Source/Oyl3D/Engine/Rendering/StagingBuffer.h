#pragma once

#include <Core/PImpl.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct StagingBufferId : OpaqueHandle<StagingBufferId> {};

	class StagingBuffer : public IDeviceObject<StagingBufferId>
	{
	protected:
		StagingBuffer() = default;

		DEFAULT_MOVE(StagingBuffer);

	public:
		NO_COPY(StagingBuffer);

		virtual
		~StagingBuffer() = default;

		virtual
		bool
		CopyMemory(const void* a_src, uint32 a_size) = 0;
	};

	using StagingBufferHandle = PImpl<StagingBuffer>;
}
