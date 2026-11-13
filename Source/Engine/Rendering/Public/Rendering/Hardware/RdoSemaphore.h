#pragma once

#include <Core/Types/Primitives.h>

#include "RenderDeviceObject.h"

namespace Oyl
{
	struct RdoSemaphoreId : IRenderDeviceObjectId<RdoSemaphoreId> {};

	class IRdoSemaphore : public IRenderDeviceObject<RdoSemaphoreId>
	{
	protected:
		IRdoSemaphore() = default;

		DEFAULT_MOVE(IRdoSemaphore);

	public:
		NO_COPY(IRdoSemaphore);

		virtual
		~IRdoSemaphore() = 0;
	};

	inline
	IRdoSemaphore::~IRdoSemaphore() {}

	using RdoSemaphoreHandle = IRenderDeviceObjectHandle<IRdoSemaphore>;
}
