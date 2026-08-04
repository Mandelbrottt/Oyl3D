#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "CommandQueue.h"
#include "Image.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class DeviceImpl : public IUniqueHandle
	{
	protected:
		DeviceImpl() = default;

	public:
		NO_MOVE(DeviceImpl);
		NO_COPY(DeviceImpl);

		virtual
		~DeviceImpl() = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		const IWindow*
		GetWindow() const = 0;

		virtual
		const CommandQueueImpl*
		GetCommandQueue(CommandQueueFlagBits a_flag) const = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		CommandBuffer
		CreateCommandBuffer(const CommandBufferImpl::CreateParams& a_params) const = 0;

		virtual
		CommandPool
		CreateCommandPool(const CommandPoolImpl::CreateParams& a_params) const = 0;

		virtual
		Image
		CreateImage(const ImageImpl::CreateParams& a_params) const = 0;

		virtual
		Shader
		CreateShader(const ShaderImpl::CreateParams& a_params) const = 0;

		virtual
		VertexBuffer
		CreateVertexBuffer(const VertexBufferImpl::CreateParams& a_params) const = 0;

		virtual
		Semaphore
		CreateSemaphore() const = 0;

		virtual
		Fence
		CreateFence() const = 0;
	};

	using Device = PImpl<DeviceImpl>;
}
