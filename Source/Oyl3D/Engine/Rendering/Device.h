#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>
#include <Core/UniquePtr.h>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "CommandQueue.h"
#include "Image.h"
#include "PresentTarget.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class Device : public IUniqueHandle
	{
	protected:
		Device() = default;

	public:
		NO_MOVE(Device);
		NO_COPY(Device);

		virtual
		~Device() = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		const CommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		CommandBufferHandle
		CreateCommandBuffer(const CommandBuffer::CreateParams& a_params) const = 0;

		virtual
		CommandPoolHandle
		CreateCommandPool(const CommandPool::CreateParams& a_params) const = 0;

		virtual
		ImageHandle
		CreateImage(const Image::CreateParams& a_params) const = 0;

		virtual
		ShaderHandle
		CreateShader(const Shader::CreateParams& a_params) const = 0;

		virtual
		VertexBufferHandle
		CreateVertexBuffer(const VertexBuffer::CreateParams& a_params) const = 0;

		virtual
		SemaphoreHandle
		CreateSemaphore() const = 0;

		virtual
		FenceHandle
		CreateFence() const = 0;
	};

	using DeviceHandle = UniquePtrImplicitConvertible<Device>;
}
