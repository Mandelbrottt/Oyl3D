#pragma once

#include <Application/ApplicationWindow.h>

#include <Rendering/SwapChain.h>

#include "VulkanPresentTarget.h"

namespace Oyl
{
	class VulkanDevice;

	struct VulkanSwapChainCreateParams
	{
		const IApplicationWindow& window;
		const VulkanPresentTarget& presentTarget;
	};

	class VULKANMODULE_EXPORT VulkanSwapChain : public ISwapChain
	{
	public:
		VulkanSwapChain(nullptr_t) {}

		VulkanSwapChain(const VulkanDevice& a_device, const VulkanSwapChainCreateParams& a_params);

		NO_COPY(VulkanSwapChain);
		DEFAULT_MOVE(VulkanSwapChain);

		virtual
		~VulkanSwapChain();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkSwapChain; }

		void
		Recreate() override;

		void
		AddOnRecreateCallback(OnRecreateDelegate a_callback) override
		{
			m_onRecreate += a_callback;
		}

		void
		RemoveOnRecreateCallback(OnRecreateDelegate a_callback) override
		{
			m_onRecreate -= a_callback;
		}

		bool
		AcquireNextImage(SemaphoreId a_semaphore, FenceId a_fence) override;

		Vector2u
		GetSize() const override;

		uint32
		GetMinImageCount() const override;

		uint32
		GetImageCount() const override;

		uint32
		GetCurrentImageIndex() const override;

		ImageId
		GetImageId(uint32 a_index) const override;

		ImageId
		GetCurrentImageId() const override;

		const vk::raii::SwapchainKHR&
		GetVkSwapChain() const { return m_vkSwapChain; }

		const Array<vk::Image>&
		GetVkImages() const { return m_vkSwapChainImageHandles; }

		vk::Image
		GetCurrentVkImage() const;

		const Array<vk::raii::ImageView>&
		GetVkImageViews() const { return m_vkSwapChainImageViews; }

		const vk::raii::ImageView*
		GetCurrentVkImageView() const;

		const vk::SurfaceFormatKHR&
		GetVkSurfaceFormat() const { return m_vkSwapChainSurfaceFormat; }

		const vk::Extent2D&
		GetVkExtent() const { return m_vkSwapChainExtent; }

	private:
		void
		CreateSwapChain();

		void
		CreateSwapChainImageViews();

		void
		CleanupSwapChain();

	private:
		const VulkanDevice* m_device = nullptr;
		const IApplicationWindow* m_window = nullptr;
		const VulkanPresentTarget* m_presentTarget = nullptr;

		vk::raii::SwapchainKHR m_vkSwapChain = nullptr;
		Array<vk::Image> m_vkSwapChainImageHandles;
		Array<vk::raii::ImageView> m_vkSwapChainImageViews;
		vk::SurfaceFormatKHR m_vkSwapChainSurfaceFormat;
		vk::Extent2D m_vkSwapChainExtent;

		uint32 m_minImageCount = 0;
		uint32 m_currentImageIndex = 0;

		MulticastDelegate<void(const ISwapChain&)> m_onRecreate;
	};
}
