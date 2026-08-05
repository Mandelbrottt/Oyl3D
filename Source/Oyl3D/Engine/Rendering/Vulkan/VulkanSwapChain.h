#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanFence.h"
#include "VulkanImage.h"
#include "VulkanSemaphore.h"

#include "Rendering/SwapChain.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class VulkanDevice;

	class OYL_RENDERING_API VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(nullptr_t);

		struct CreateParams
		{
			const IWindow& window;
		};

		VulkanSwapChain(const VulkanDevice& a_device, const CreateParams& a_params);

		VulkanSwapChain(VulkanSwapChain&& a_other) noexcept;
		VulkanSwapChain&
		operator =(VulkanSwapChain&& a_other) noexcept;

		virtual
		~VulkanSwapChain();

		void
		Destroy() override;

		bool
		IsValid() const override;

		Vector2u
		GetSize() const override;

		void
		Recreate() override;

		bool
		AcquireNextImage(SemaphoreId a_semaphore, FenceId a_fence) override
		{
			return AcquireNextImage((VulkanSemaphoreId) a_semaphore, (VulkanFenceId) a_fence);
		}

		bool
		AcquireNextImage(const VulkanSemaphore& a_semaphore, const VulkanFence& a_fence)
		{
			return AcquireNextImage(a_semaphore.GetId(), a_fence.GetId());
		}

		bool
		AcquireNextImage(VulkanSemaphoreId a_semaphore, VulkanFenceId a_fence);

		uint32
		GetImageCount() const override;

	protected:
		ImageId
		GetImageId(uint32 a_index) const override
		{
			return GetVulkanImageId(a_index);
		}

	public:
		VulkanImageId
		GetVulkanImageId(uint32 a_index) const;

		uint32
		GetCurrentImageIndex() const override;

	protected:
		ImageId
		GetCurrentImageId() const override
		{
			return GetCurrentVulkanImageId();
		}

	public:
		VulkanImageId
		GetCurrentVulkanImageId() const;

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

	using VulkanSwapChainHandle = PImpl<VulkanSwapChain>;
}
