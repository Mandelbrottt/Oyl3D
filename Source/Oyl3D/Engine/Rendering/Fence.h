#pragma once

#include <Core/Typedefs.h>
#include <Core/UniqueHandle.h>

namespace Oyl::Rendering
{
	struct FenceHandle : OpaqueHandle<FenceHandle> {};

	class IFence : public IUniqueHandle
	{
	protected:
		IFence() = default;

		DEFAULT_MOVE(IFence);

	public:
		NO_COPY(IFence);

		virtual
		~IFence() = default;

		FenceHandle
		GetHandle() const
		{
			return GetHandleImpl();
		}

		operator FenceHandle() const
		{
			return GetHandle();
		}

		virtual
		void
		Reset() = 0;

		virtual
		bool
		Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) = 0;

	protected:
		virtual
		FenceHandle
		GetHandleImpl() const = 0;
	};
}
