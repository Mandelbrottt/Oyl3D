#pragma once

#include <Core/PImpl.h>

#include "Rendering/CommandPool.h"
#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class CommandPool;
}

namespace Oyl::Rendering
{
	class VulkanDevice;

	class OYL_RENDERING_API VulkanCommandPool : public CommandPool
	{
	public:
		VulkanCommandPool(nullptr_t);

		VulkanCommandPool(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanCommandPool(VulkanCommandPool&& a_other) noexcept;
		VulkanCommandPool&
		operator =(VulkanCommandPool&& a_other) noexcept;

		virtual
		~VulkanCommandPool();

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

	using VulkanCommandPoolHandle = PImpl<VulkanCommandPool>;
}
