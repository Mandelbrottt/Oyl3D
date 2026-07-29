#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct StagingBufferHandle : OpaqueHandle<StagingBufferHandle> {};

	class StagingBuffer : public IDeviceObject<StagingBufferHandle>
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
}
