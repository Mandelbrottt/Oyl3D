#pragma once

#include <Core/Containers/Array.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct UniformBufferId : IDeviceObjectId<UniformBufferId> {};

	struct UniformBufferCreateParams
	{
		uint64 stride;
	};

	class IUniformBuffer : public IDeviceObject<UniformBufferId>
	{
	protected:
		IUniformBuffer() = default;
		DEFAULT_MOVE(IUniformBuffer);

	public:
		NO_COPY(IUniformBuffer);

		virtual
		~IUniformBuffer() = 0;

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
	IUniformBuffer::~IUniformBuffer() {}

	using UniformBufferHandle = IObjectHandle<IUniformBuffer>;
}
