#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

#include "Rendering/Fence.h"

namespace Oyl::Rendering::Vulkan
{
	using FenceHandle = ImplicitConversionWrapper<FenceHandle, vk::Fence>;

	class OYL_RENDERING_API Fence : public IFence
	{
	public:
		Fence() = default;

		Fence(std::nullptr_t)
			: Fence() {}

		explicit
		Fence(const Device& a_device);

		Fence(Fence&& a_other) noexcept;
		Fence&
		operator =(Fence&& a_other) noexcept;

		virtual
		~Fence()
		{
			Fence::Destroy();
		}

		void
		Destroy() override;

		bool
		IsValid() const override
		{
			return *m_fence;
		}

		FenceHandle
		GetHandle() const
		{
			return *m_fence;
		}

		const vk::raii::Fence&
		GetVkFence() const
		{
			return m_fence;
		}

	protected:
		Rendering::FenceHandle
		GetHandleImpl() const override
		{
			return GetHandle();
		}

	private:
		vk::raii::Fence m_fence = nullptr;
	};
}
