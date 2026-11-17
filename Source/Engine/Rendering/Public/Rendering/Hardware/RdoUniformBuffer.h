#pragma once

#include "RenderDeviceObject.h"

namespace Oyl
{
	struct RdoUniformBufferId : IRenderDeviceObjectId<RdoUniformBufferId> {};

	struct RdoUniformBufferCreateParams
	{
		uint64 stride;
	};

	class IRdoUniformBuffer : public IRenderDeviceObject<RdoUniformBufferId>
	{
	protected:
		IRdoUniformBuffer() = default;
		DEFAULT_MOVE(IRdoUniformBuffer);

	public:
		NO_COPY(IRdoUniformBuffer);

		virtual
		~IRdoUniformBuffer() = 0;

		virtual
		void
		CopyToBuffer(void* a_data, uint64 a_size) = 0;

		template<typename TData>
			requires !std::is_void_v<TData>
		void
		CopyToBuffer(TData* a_data)
		{
			CopyToBuffer(static_cast<void*>(a_data), sizeof(TData));
		}
	};

	inline
	IRdoUniformBuffer::~IRdoUniformBuffer() {}

	using RdoUniformBufferHandle = IRenderDeviceObjectHandle<IRdoUniformBuffer>;
}
