#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/SwapChain.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering::Vulkan
{
	class DeviceImpl;

	class OYL_RENDERING_API SwapChainImpl : public Rendering::SwapChainImpl
	{
	public:
		SwapChainImpl(nullptr_t);

		struct CreateParams
		{
			const IWindow& window;
		};

		SwapChainImpl(const DeviceImpl& a_device, const CreateParams& a_params);

		SwapChainImpl(SwapChainImpl&& a_other) noexcept;
		SwapChainImpl&
		operator =(SwapChainImpl&& a_other) noexcept;

		virtual
		~SwapChainImpl();

		void
		Destroy() override;

		bool
		IsValid() const override;

		Vector2u
		GetSize() const override;

		void
		Recreate() override;

		bool
		AcquireNextImage(Rendering::SemaphoreHandle a_semaphore, Rendering::FenceHandle a_fence) override
		{
			return AcquireNextImage((SemaphoreHandle) a_semaphore, (FenceHandle) a_fence);
		}

		bool
		AcquireNextImage(const SemaphoreImpl& a_semaphore, const FenceImpl& a_fence)
		{
			return AcquireNextImage(a_semaphore.GetHandle(), a_fence.GetHandle());
		}

		bool
		AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fence);

		uint32
		GetCurrentImageIndex() const override;

		uint32
		GetImageCount() const override;

		const vk::raii::SwapchainKHR&
		GetVkSwapChain() const;

		const std::vector<vk::Image>&
		GetVkImages() const;

		vk::Image
		GetCurrentVkImage() const;

		const std::vector<vk::raii::ImageView>&
		GetVkImageViews() const;

		const vk::raii::ImageView&
		GetCurrentVkImageView() const;

		const vk::SurfaceFormatKHR&
		GetVkSurfaceFormat() const;

		const vk::Extent2D&
		GetVkExtent() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using SwapChain = PImpl<SwapChainImpl>;
}
