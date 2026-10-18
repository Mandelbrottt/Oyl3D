#include "VulkanSwapChain.h"

#include <Core/Profiling/Profiling.h>

namespace
{
	vk::SurfaceFormatKHR
	ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& a_availableFormats);

	vk::PresentModeKHR
	ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& a_availablePresentModes);

	vk::Extent2D
	ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& a_capabilities, Oyl::Vector2u a_frameBufferSize);

	uint32
	ChooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& a_capabilities);
}

namespace Oyl
{
	VulkanSwapChain::VulkanSwapChain(const VulkanDevice& a_device, const VulkanSwapChainCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		m_device = &a_device;
		m_window = &a_params.window;
		m_presentTarget = &a_params.presentTarget;

		CreateSwapChain();
		CreateSwapChainImageViews();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		VulkanSwapChain::Destroy();
	}

	void
	VulkanSwapChain::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		CleanupSwapChain();
	}

	Vector2u
	VulkanSwapChain::GetSize() const
	{
		return Vector2u(m_vkSwapChainExtent.width, m_vkSwapChainExtent.height);
	}

	void
	VulkanSwapChain::Recreate()
	{
		OYL_PROFILE_FUNCTION();

		m_device->WaitUntilIdle();

		CleanupSwapChain();

		// If FrameBuffer has 0 size, don't try to recreate SwapChain
		auto frameBufferSize = m_window->GetContentSize();
		if (frameBufferSize.x == 0 || frameBufferSize.y == 0)
		{
			return;
		}

		CreateSwapChain();
		CreateSwapChainImageViews();
	}

	vk::Image
	VulkanSwapChain::GetCurrentVkImage() const
	{
		if (m_currentImageIndex > 0 && m_currentImageIndex < m_vkSwapChainImageHandles.Length())
			return m_vkSwapChainImageHandles[m_currentImageIndex];

		return {};
	}

	const vk::raii::ImageView*
	VulkanSwapChain::GetCurrentVkImageView() const
	{
		if (m_currentImageIndex > 0 && m_currentImageIndex < m_vkSwapChainImageHandles.Length())
			return &m_vkSwapChainImageViews[m_currentImageIndex];

		return {};
	}

	void
	VulkanSwapChain::CreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		auto frameBufferSize = m_window->GetContentSize();
		uint32 width = frameBufferSize.x;
		uint32 height = frameBufferSize.y;

		const auto& physicalDevice = m_device->GetVkPhysicalDevice();
		const auto& surface = m_presentTarget->GetVkSurface();

		vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
		m_vkSwapChainExtent = ChooseSwapExtent(surfaceCapabilities, { width, height });
		uint32_t minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		m_vkSwapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo {
			.surface = *surface,
			.minImageCount = minImageCount,
			.imageFormat = m_vkSwapChainSurfaceFormat.format,
			.imageColorSpace = m_vkSwapChainSurfaceFormat.colorSpace,
			.imageExtent = m_vkSwapChainExtent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment
			              | vk::ImageUsageFlagBits::eTransferDst,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = presentMode,
			.clipped = true,
			.oldSwapchain = nullptr,
		};
		m_vkSwapChain = vk::raii::SwapchainKHR(m_device->GetVkDevice(), swapChainCreateInfo);
		m_vkSwapChainImageHandles = m_vkSwapChain.getImages();
	}

	void
	VulkanSwapChain::CreateSwapChainImageViews()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(m_vkSwapChainImageViews.empty());

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.viewType = vk::ImageViewType::e2D,
			.format = m_vkSwapChainSurfaceFormat.format,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		for (auto& image : m_vkSwapChainImageHandles)
		{
			imageViewCreateInfo.image = image;
			m_vkSwapChainImageViews.emplace_back(m_device->GetVkDevice(), imageViewCreateInfo);
		}
	}

	void
	VulkanSwapChain::CleanupSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		m_vkSwapChainImageViews.Clear();
		m_vkSwapChain = nullptr;
	}
}

namespace
{
	vk::SurfaceFormatKHR
	ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& a_availableFormats)
	{
		OYL_ASSERT(!a_availableFormats.empty());

		const auto formatIt = std::ranges::find_if(
			a_availableFormats,
			[](const auto& a_format)
			{
				return a_format.format == vk::Format::eB8G8R8A8Srgb
				       && a_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
			}
		);

		// Valid format not found, bail
		if (formatIt == a_availableFormats.end())
		{
			return a_availableFormats[0];
		}
		// Return found format
		return *formatIt;
	}

	vk::PresentModeKHR
	ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& a_availablePresentModes)
	{
		OYL_ASSERT(
			std::ranges::any_of(
				a_availablePresentModes,
				[](auto presentMode)
				{
				return presentMode == vk::PresentModeKHR::eFifo;
				}
			)
		);

		auto anyOfMailboxMode = std::ranges::any_of(
			a_availablePresentModes,
			[](const vk::PresentModeKHR a_value)
			{
				return vk::PresentModeKHR::eMailbox == a_value;
			}
		);

		return anyOfMailboxMode
			       ? vk::PresentModeKHR::eMailbox
			       : vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D
	ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& a_capabilities, Oyl::Vector2u a_frameBufferSize)
	{
		if (a_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return a_capabilities.currentExtent;
		}

		return {
			std::clamp<uint32_t>(a_frameBufferSize.x, a_capabilities.minImageExtent.width, a_capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(a_frameBufferSize.y, a_capabilities.minImageExtent.height, a_capabilities.maxImageExtent.height)
		};
	}

	Oyl::uint32
	ChooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& a_capabilities)
	{
		auto minImageCount = std::max(3u, a_capabilities.minImageCount);
		if ((0 < a_capabilities.maxImageCount)
		    && (a_capabilities.maxImageCount < minImageCount))
		{
			minImageCount = a_capabilities.maxImageCount;
		}
		return minImageCount;
	}
}
