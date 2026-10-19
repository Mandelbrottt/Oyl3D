#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>

#include "CommandBuffer.h"
#include "CommandQueue.h"
#include "CommandPool.h"
#include "Image.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Semaphore.h"
#include "Fence.h"
#include "RenderingEnums.h"

namespace Oyl
{
	class IDevice : public IDestroyable
	{
	protected:
		IDevice() = default;

		DEFAULT_MOVE(IDevice);

	public:
		NO_COPY(IDevice);

		virtual
		~IDevice() = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		const ICommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const = 0;

		virtual
		ICommandBuffer*
		CreateCommandBuffer(const CommandBufferCreateParams& a_params) const = 0;

		virtual
		ICommandPool*
		CreateCommandPool(const CommandPoolCreateParams& a_params) const = 0;

		virtual
		IImage*
		CreateImage(const ImageCreateParams& a_params) const = 0;

		virtual
		IShader*
		CreateShader(const ShaderCreateParams& a_params) const = 0;

		virtual
		IVertexBuffer*
		CreateVertexBuffer(const VertexBufferCreateParams& a_params) const = 0;

		virtual
		ISemaphore*
		CreateSemaphore() const = 0;

		virtual
		IFence*
		CreateFence() const = 0;
	};

	inline
	IDevice::~IDevice() {}
}
