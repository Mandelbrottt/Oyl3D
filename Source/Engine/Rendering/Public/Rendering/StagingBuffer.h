#pragma once

#include <Core/Types/Primitives.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct StagingBufferId : IDeviceObjectId<StagingBufferId> {};

	class IStagingBuffer : public IDeviceObject<StagingBufferId>
	{
	protected:
		IStagingBuffer() = default;

		DEFAULT_MOVE(IStagingBuffer);

	public:
		NO_COPY(IStagingBuffer);

		virtual
		~IStagingBuffer() = 0;

		virtual
		bool
		CopyMemory(const void* a_src, uint32 a_size) = 0;
	};

	inline
	IStagingBuffer::~IStagingBuffer() {}

	using StagingBufferHandle = IObjectHandle<IStagingBuffer>;
}
