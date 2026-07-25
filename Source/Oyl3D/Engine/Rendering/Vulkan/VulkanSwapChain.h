#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/SwapChain.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API SwapChain : public Rendering::SwapChain
	{
	public:
		SwapChain();

		struct CreateParams
		{
			const IWindow* window;
			const Device* device;
		};

		explicit
		SwapChain(const CreateParams& a_params);

		SwapChain(SwapChain&& a_other) noexcept;
		SwapChain&
		operator =(SwapChain&& a_other) noexcept;

		virtual
		~SwapChain();

		void
		Destroy() override;

		bool
		IsValid() const override;

		void
		Recreate() override;

		bool
		AcquireNextImage(Rendering::SemaphoreHandle a_semaphore, Rendering::FenceHandle a_fence) override
		{
			return AcquireNextImage((SemaphoreHandle) a_semaphore, (FenceHandle) a_fence);
		}

		bool
		AcquireNextImage(const Semaphore& a_semaphore, const Fence& a_fence)
		{
			return AcquireNextImage(a_semaphore.GetHandle(), a_fence.GetHandle());
		}

		bool
		AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fence);

		uint32
		GetCurrentImageIndex() const override;

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
}
