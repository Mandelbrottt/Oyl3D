#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>

#include "CommandQueue.h"
#include "RenderingEnums.h"

namespace Oyl
{
	class IApplicationWindow;
}

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

		//virtual
		//CommandBufferHandle
		//CreateCommandBuffer(const CommandBuffer::CreateParams& a_params) const = 0;

		//virtual
		//CommandPoolHandle
		//CreateCommandPool(const CommandPool::CreateParams& a_params) const = 0;

		//virtual
		//ImageHandle
		//CreateImage(const Image::CreateParams& a_params) const = 0;

		//virtual
		//ShaderHandle
		//CreateShader(const Shader::CreateParams& a_params) const = 0;

		//virtual
		//VertexBufferHandle
		//CreateVertexBuffer(const VertexBuffer::CreateParams& a_params) const = 0;

		//virtual
		//SemaphoreHandle
		//CreateSemaphore() const = 0;

		//virtual
		//FenceHandle
		//CreateFence() const = 0;
	};

	inline
	IDevice::~IDevice() {}
}
