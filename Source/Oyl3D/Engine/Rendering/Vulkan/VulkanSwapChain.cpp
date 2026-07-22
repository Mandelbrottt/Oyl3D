#include "VulkanSwapChain.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

#include "Rendering/Glfw/GlfwWindow.h"

namespace
{
	vk::SurfaceFormatKHR
	ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& a_availableFormats);

	vk::PresentModeKHR
	ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& a_availablePresentModes);

	vk::Extent2D
	ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& a_capabilities, Oyl::Vector2u a_frameBufferSize);

	Oyl::uint32
	ChooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& a_capabilities);
}

namespace Oyl::Rendering::Vulkan
{
	struct SwapChain::Impl
	{
		const Device* device = nullptr;
		const Window* window = nullptr;

		vk::raii::SwapchainKHR swapChain = nullptr;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::raii::ImageView> swapChainImageViews;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		vk::Extent2D swapChainExtent;

		void
		CreateSwapChain();
		void
		CreateSwapChainImageViews();
		void
		CleanupSwapChain();
		void
		RecreateSwapChain();
	};

	SwapChain::SwapChain()
		: m_impl(nullptr) {}

	SwapChain::SwapChain(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->device = a_params.device;
		m_impl->window = a_params.window;

		m_impl->CreateSwapChain();
		m_impl->CreateSwapChainImageViews();
	}

	SwapChain::SwapChain(SwapChain&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	SwapChain&
	SwapChain::operator=(SwapChain&& a_other) noexcept
	{
		if (this == &a_other)
			return *this;

		std::swap(m_impl, a_other.m_impl);
		return *this;
	}

	SwapChain::~SwapChain()
	{
		SwapChain::Destroy();
	}

	bool
	SwapChain::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return Rendering::SwapChain::Destroy();

		m_impl->CleanupSwapChain();
		m_impl.release();

		return Rendering::SwapChain::Destroy();
	}

	void
	SwapChain::Recreate()
	{
		OYL_PROFILE_FUNCTION();

		m_impl->RecreateSwapChain();
	}

	bool
	SwapChain::IsValid() const
	{
		return m_impl && m_impl->swapChain != nullptr;
	}

	const vk::raii::SwapchainKHR&
	SwapChain::GetVkSwapChain() const
	{
		return m_impl->swapChain;
	}

	const std::vector<vk::Image>&
	SwapChain::GetVkImages() const
	{
		return m_impl->swapChainImages;
	}

	const std::vector<vk::raii::ImageView>&
	SwapChain::GetVkImageViews() const
	{
		return m_impl->swapChainImageViews;
	}

	const vk::SurfaceFormatKHR&
	SwapChain::GetVkSurfaceFormat() const
	{
		return m_impl->swapChainSurfaceFormat;
	}

	const vk::Extent2D&
	SwapChain::GetVkExtent() const
	{
		return m_impl->swapChainExtent;
	}

	void
	SwapChain::Impl::CreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		auto frameBufferSize = window->GetFrameBufferSize();
		uint32 width = frameBufferSize.x;
		uint32 height = frameBufferSize.y;

		const auto& physicalDevice = device->GetVkPhysicalDevice();
		const auto& surface = device->GetVkSurface();

		vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
		swapChainExtent = ChooseSwapExtent(surfaceCapabilities, { width, height });
		uint32_t minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		swapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo {
			.surface = *surface,
			.minImageCount = minImageCount,
			.imageFormat = swapChainSurfaceFormat.format,
			.imageColorSpace = swapChainSurfaceFormat.colorSpace,
			.imageExtent = swapChainExtent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = presentMode,
			.clipped = true,
			.oldSwapchain = nullptr
		};
		swapChain = vk::raii::SwapchainKHR(device->GetVkDevice(), swapChainCreateInfo);
		swapChainImages = swapChain.getImages();
	}

	void
	SwapChain::Impl::CreateSwapChainImageViews()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(swapChainImageViews.empty());

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.viewType = vk::ImageViewType::e2D,
			.format = swapChainSurfaceFormat.format,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		for (auto& image : swapChainImages)
		{
			imageViewCreateInfo.image = image;
			swapChainImageViews.emplace_back(device->GetVkDevice(), imageViewCreateInfo);
		}
	}

	void
	SwapChain::Impl::CleanupSwapChain()
	{
		swapChainImageViews.clear();
		swapChain = nullptr;
	}

	void
	SwapChain::Impl::RecreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		device->GetVkDevice().waitIdle();

		CleanupSwapChain();

		// If FrameBuffer has 0 size, don't try to recreate SwapChain
		auto frameBufferSize = window->GetFrameBufferSize();
		if (frameBufferSize.x == 0 || frameBufferSize.y == 0)
		{
			return;
		}

		CreateSwapChain();
		CreateSwapChainImageViews();
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
