#pragma once

#include <Core/PImpl.h>

#include "Rendering/CommandPool.h"
#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class CommandPool;
}

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;

	class OYL_RENDERING_API CommandPoolImpl : public Rendering::CommandPoolImpl
	{
	public:
		CommandPoolImpl(nullptr_t);

		CommandPoolImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		CommandPoolImpl(CommandPoolImpl&& a_other) noexcept;
		CommandPoolImpl&
		operator =(CommandPoolImpl&& a_other) noexcept;

		virtual
		~CommandPoolImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::CommandPool&
		GetVkCommandPool() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using CommandPool = PImpl<CommandPoolImpl>;
}
