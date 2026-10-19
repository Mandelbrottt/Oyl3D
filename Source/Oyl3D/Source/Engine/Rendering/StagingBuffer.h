#pragma once

#include "IDeviceObject.h"

#include <Core/Types/Primitives.h>

namespace Oyl
{
	struct StagingBufferId : IDeviceObjectId<StagingBufferId> {};

	class StagingBuffer : public IDeviceObject<StagingBufferId>
	{
	protected:
		StagingBuffer() = default;

		DEFAULT_MOVE(StagingBuffer);

	public:
		NO_COPY(StagingBuffer);

		virtual
		~StagingBuffer() = 0;

		virtual
		bool
		CopyMemory(const void* a_src, uint32 a_size) = 0;
	};

	inline
	StagingBuffer::~StagingBuffer() {}
}
