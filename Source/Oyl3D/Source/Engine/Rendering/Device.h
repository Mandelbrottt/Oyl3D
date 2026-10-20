#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "CommandQueue.h"
#include "Fence.h"
#include "IDeviceObject.h"
#include "Image.h"
#include "RenderingEnums.h"
#include "Semaphore.h"
#include "Shader.h"
#include "VertexBuffer.h"

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
		CommandBufferHandle
		CreateCommandBuffer(const CommandBufferCreateParams& a_params) const = 0;

		virtual
		CommandPoolHandle
		CreateCommandPool(const CommandPoolCreateParams& a_params) const = 0;

		virtual
		ImageHandle
		CreateImage(const ImageCreateParams& a_params) const = 0;

		virtual
		ShaderHandle
		CreateShader(const ShaderCreateParams& a_params) const = 0;

		virtual
		VertexBufferHandle
		CreateVertexBuffer(const VertexBufferCreateParams& a_params) const = 0;

		virtual
		SemaphoreHandle
		CreateSemaphore() const = 0;

		virtual
		FenceHandle
		CreateFence() const = 0;
	};

	inline
	IDevice::~IDevice() {}

	using DeviceHandle = IObjectHandle<IDevice>;
}
