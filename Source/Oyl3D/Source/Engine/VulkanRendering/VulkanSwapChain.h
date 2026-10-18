#pragma once

#include <Application/ApplicationWindow.h>

#include <Rendering/SwapChain.h>

#include "VulkanDevice.h"
#include "VulkanPresentTarget.h"

namespace Oyl
{
	struct VulkanSwapChainCreateParams
	{
		const IApplicationWindow& window;
		const VulkanPresentTarget& presentTarget;
	};

	class VulkanSwapChain : public ISwapChain
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

		Vector2u
		GetSize() const override;

		void
		Recreate() override;

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

		uint32 m_currentImageIndex = 0;
	};
}
