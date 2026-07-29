#pragma once

#include "Rendering/CommandPool.h"
#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class CommandPool;
}

namespace Oyl::Rendering::Vulkan
{
	class Device;

	class OYL_RENDERING_API CommandPool : public ICommandPool
	{
	public:
		CommandPool();

		struct CreateParams
		{
			CommandQueueFlagBits commandQueueFlags;
		};

		explicit
		CommandPool(const Device& a_device, const CreateParams& a_params);

		CommandPool(CommandPool&& a_other) noexcept;
		CommandPool&
		operator =(CommandPool&& a_other) noexcept;

		virtual
		~CommandPool();

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
}
