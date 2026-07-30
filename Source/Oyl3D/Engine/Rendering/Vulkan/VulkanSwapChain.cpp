#include "VulkanSwapChain.h"

#include <Core/Math/Vector.h>

#include "VulkanDevice.h"
#include "VulkanCommandQueue.h"

#include "Rendering/Window.h"

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

namespace Oyl::Rendering::Vulkan
{
	struct SwapChain::Impl
	{
		const DeviceImpl* device = nullptr;
		const IWindow* window = nullptr;

		vk::raii::SwapchainKHR vkSwapChain = nullptr;
		std::vector<vk::Image> vkSwapChainImageHandles;
		std::vector<vk::raii::ImageView> vkSwapChainImageViews;
		vk::SurfaceFormatKHR vkSwapChainSurfaceFormat;
		vk::Extent2D vkSwapChainExtent;

		uint32 imageIndex;

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

	SwapChain::SwapChain(const DeviceImpl& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->device = &a_device;
		m_impl->window = &a_params.window;

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
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	SwapChain::~SwapChain()
	{
		SwapChain::Destroy();
	}

	void
	SwapChain::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_impl->CleanupSwapChain();
		m_impl.release();
	}

	bool
	SwapChain::IsValid() const
	{
		return m_impl
		       && *m_impl->vkSwapChain;
	}

	Vector2u
	SwapChain::GetSize() const
	{
		return Vector2u(m_impl->vkSwapChainExtent.width, m_impl->vkSwapChainExtent.height);
	}

	void
	SwapChain::Recreate()
	{
		OYL_PROFILE_FUNCTION();

		m_impl->RecreateSwapChain();
	}

	bool
	SwapChain::AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fence)
	{
		OYL_PROFILE_FUNCTION();

		auto [result, imageIndex] = m_impl->vkSwapChain.acquireNextImage(UINT64_MAX, a_semaphore, a_fence);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			return false;
		}
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			OYL_ASSERT(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		m_impl->imageIndex = imageIndex;
		return true;
	}

	uint32
	SwapChain::GetCurrentImageIndex() const
	{
		return m_impl->imageIndex;
	}

	const vk::raii::SwapchainKHR&
	SwapChain::GetVkSwapChain() const
	{
		return m_impl->vkSwapChain;
	}

	const std::vector<vk::Image>&
	SwapChain::GetVkImages() const
	{
		return m_impl->vkSwapChainImageHandles;
	}

	vk::Image
	SwapChain::GetCurrentVkImage() const
	{
		return m_impl->vkSwapChainImageHandles[m_impl->imageIndex];
	}

	const std::vector<vk::raii::ImageView>&
	SwapChain::GetVkImageViews() const
	{
		return m_impl->vkSwapChainImageViews;
	}

	const vk::raii::ImageView&
	SwapChain::GetCurrentVkImageView() const
	{
		return m_impl->vkSwapChainImageViews[m_impl->imageIndex];
	}

	const vk::SurfaceFormatKHR&
	SwapChain::GetVkSurfaceFormat() const
	{
		return m_impl->vkSwapChainSurfaceFormat;
	}

	const vk::Extent2D&
	SwapChain::GetVkExtent() const
	{
		return m_impl->vkSwapChainExtent;
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
		vkSwapChainExtent = ChooseSwapExtent(surfaceCapabilities, { width, height });
		uint32_t minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		vkSwapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo {
			.surface = *surface,
			.minImageCount = minImageCount,
			.imageFormat = vkSwapChainSurfaceFormat.format,
			.imageColorSpace = vkSwapChainSurfaceFormat.colorSpace,
			.imageExtent = vkSwapChainExtent,
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
		vkSwapChain = vk::raii::SwapchainKHR(device->GetVkDevice(), swapChainCreateInfo);
		vkSwapChainImageHandles = vkSwapChain.getImages();
	}

	void
	SwapChain::Impl::CreateSwapChainImageViews()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(vkSwapChainImageViews.empty());

		vk::ImageViewCreateInfo imageViewCreateInfo {
			.viewType = vk::ImageViewType::e2D,
			.format = vkSwapChainSurfaceFormat.format,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.layerCount = 1
			},
		};
		for (auto& image : vkSwapChainImageHandles)
		{
			imageViewCreateInfo.image = image;
			vkSwapChainImageViews.emplace_back(device->GetVkDevice(), imageViewCreateInfo);
		}
	}

	void
	SwapChain::Impl::CleanupSwapChain()
	{
		vkSwapChainImageViews.clear();
		vkSwapChain = nullptr;
	}

	void
	SwapChain::Impl::RecreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		device->WaitUntilIdle();

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
