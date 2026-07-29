#pragma once

#include <Core/EnumFlags.h>
#include <Core/UniqueHandle.h>

#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class ICommandBuffer;
	class ISwapChain;

	enum class CommandQueueFlagBits : uint32
	{
		Graphics = 1 << 0,
		Compute = 1 << 1,
		Transfer = 1 << 2,
	};

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(CommandQueueFlagBits)

	using CommandQueueFlags = EnumFlags<CommandQueueFlagBits>;

	class ICommandQueue : public IUniqueHandle
	{
	public:
		struct CreateParams {};

	protected:
		ICommandQueue() = default;

		DEFAULT_MOVE(ICommandQueue);

	public:
		NO_COPY(ICommandQueue);

		virtual
		~ICommandQueue() = default;

		struct SubmitParams
		{
			const ICommandBuffer& commandBuffer;
			SemaphoreHandle waitSemaphore;
			SemaphoreHandle signalSemaphore;
			FenceHandle fence;
		};

		virtual
		void
		Submit(const SubmitParams& a_params) = 0;

		struct PresentParams
		{
			SemaphoreHandle waitSemaphore;
			const ISwapChain& swapChain;
		};

		virtual
		void
		Present(const PresentParams& a_params) = 0;
	};
}
