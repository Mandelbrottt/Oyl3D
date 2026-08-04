#pragma once

#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class Queue;
}

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;
	class CommandBufferImpl;
	class SwapChainImpl;

	class OYL_RENDERING_API CommandQueueImpl : public Rendering::CommandQueueImpl
	{
	public:
		CommandQueueImpl(nullptr_t);

		CommandQueueImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		NO_COPY(CommandQueueImpl);

		CommandQueueImpl(CommandQueueImpl&& a_other) noexcept;
		CommandQueueImpl&
		operator =(CommandQueueImpl&& a_other) noexcept;

		virtual
		~CommandQueueImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Queue&
		GetVkQueue() const;

		uint32
		GetVkQueueFamilyIndex() const;

		void
		WaitUntilIdle() const override;

		bool
		Submit(const SubmitParams& a_params) const override;

		bool
		Present(const PresentParams& a_params) const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using CommandQueue = PImpl<CommandQueueImpl>;
}
