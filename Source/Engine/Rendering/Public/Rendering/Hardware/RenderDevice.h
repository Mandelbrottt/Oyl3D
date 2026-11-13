#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>

#include "RdoCommandBuffer.h"
#include "RdoCommandPool.h"
#include "RdoCommandQueue.h"
#include "RdoFence.h"
#include "RdoImage.h"
#include "RenderDeviceObject.h"
#include "RdoSemaphore.h"
#include "RdoShader.h"
#include "RdoUniformBuffer.h"
#include "RdoVertexBuffer.h"

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	class IRenderDevice : public IDestroyable
	{
	protected:
		IRenderDevice() = default;

		DEFAULT_MOVE(IRenderDevice);

	public:
		NO_COPY(IRenderDevice);

		virtual
		~IRenderDevice() = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		const IRdoCommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const = 0;

		virtual
		RdoCommandBufferHandle
		CreateCommandBuffer(const RdoCommandBufferCreateParams& a_params) const = 0;

		virtual
		RdoCommandPoolHandle
		CreateCommandPool(const RdoCommandPoolCreateParams& a_params) const = 0;

		virtual
		RdoImageHandle
		CreateImage(const RdoImageCreateParams& a_params) const = 0;

		virtual
		RdoShaderHandle
		CreateShader(const RdoShaderCreateParams& a_params) const = 0;

		virtual
		RdoVertexBufferHandle
		CreateVertexBuffer(const RdoVertexBufferCreateParams& a_params) const = 0;

		virtual
		RdoUniformBufferHandle
		CreateUniformBuffer(const RdoUniformBufferCreateParams& a_params) const = 0;

		virtual
		RdoSemaphoreHandle
		CreateSemaphore() const = 0;

		virtual
		RdoFenceHandle
		CreateFence() const = 0;
	};

	inline
	IRenderDevice::~IRenderDevice() {}

	using RenderDeviceHandle = IRenderDeviceObjectHandle<IRenderDevice>;
}
