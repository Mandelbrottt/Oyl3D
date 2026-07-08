#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "Core/Logging/Logging.h"

#include "Rendering/Window/Window.h"

static const std::vector VALIDATION_LAYERS {
	"VK_LAYER_KHRONOS_validation",
};

constexpr bool ENABLE_VALIDATION_LAYERS =
#if defined(OYL_DISTRIBUTION)
	false;
#else
	true;
#endif

static const std::vector REQUIRED_DEVICE_EXTENSION {
	vk::KHRSwapchainExtensionName,
};

namespace
{
	VKAPI_ATTR
	vk::Bool32
	VKAPI_CALL
	DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT a_severity,
		vk::DebugUtilsMessageTypeFlagsEXT a_type,
		const vk::DebugUtilsMessengerCallbackDataEXT* a_pCallbackData,
		void* a_pUserData
	);

	bool
	IsDeviceSuitable(const vk::raii::PhysicalDevice& a_physicalDevice);

	std::vector<const char*>
	GetRequiredInstanceExtensions();

	vk::SurfaceFormatKHR
	ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& a_availableFormats);

	vk::PresentModeKHR
	ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& a_availablePresentModes);

	vk::Extent2D
	ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& a_capabilities, Oyl::Vector2u a_frameBufferSize);

	Oyl::uint32
	ChooseSwapMinImageCount(const vk::SurfaceCapabilitiesKHR& a_capabilities);
}

namespace Oyl::Rendering
{
	struct VulkanRenderContext::Impl
	{
		Window* window;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		vk::raii::SurfaceKHR surface = nullptr;

		vk::raii::PhysicalDevice physicalDevice = nullptr;
		vk::raii::Device device = nullptr;
		vk::raii::Queue graphicsQueue = nullptr;

		vk::raii::SwapchainKHR swapChain = nullptr;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::raii::ImageView> swapChainImageViews;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		vk::Extent2D swapChainExtent;

		void
		CreateInstance();
		void
		SetupDebugMessenger();
		void
		CreateSurface();
		void
		PickPhysicalDevice();
		void
		CreateLogicalDevice();
		void
		CreateSwapChain();
		void
		CreateSwapChainImageViews();
	};

	VulkanRenderContext::VulkanRenderContext() noexcept
		: m_impl(nullptr) {}

	VulkanRenderContext::VulkanRenderContext(const RenderContextParams& a_params) noexcept
		: RenderContext(a_params),
		  m_impl(nullptr)
	{
		VulkanRenderContext::Init(a_params);
	}

	VulkanRenderContext::VulkanRenderContext(VulkanRenderContext&& a_other) noexcept
		: RenderContext(std::move(a_other)),
		  m_impl(nullptr)
	{
		m_impl.swap(a_other.m_impl);
	}

	VulkanRenderContext&
	VulkanRenderContext::operator=(VulkanRenderContext&& a_other) noexcept
	{
		RenderContext::operator=(std::move(a_other));
		new(this) VulkanRenderContext(std::move(a_other));
		return *this;
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		Destroy();
	}

	void
	VulkanRenderContext::Init(const RenderContextParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			m_impl->SetupDebugMessenger();
		m_impl->CreateSurface();
		m_impl->PickPhysicalDevice();
		m_impl->CreateLogicalDevice();
		m_impl->CreateSwapChain();
		m_impl->CreateSwapChainImageViews();
	}

	void
	VulkanRenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();
	}

	void
	VulkanRenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		*m_impl = {};
	}

	void
	VulkanRenderContext::Impl::CreateInstance()
	{
		vk::ApplicationInfo appInfo {
			.pApplicationName = "Oyl3D",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "Oyl3D",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = vk::ApiVersion14,
		};

		// Get the required layers
		std::vector<const char*> requiredLayers;
		if constexpr (ENABLE_VALIDATION_LAYERS)
		{
			requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
		}

		// Check if the required layers are supported by the Vulkan implementation.
		auto layerProperties = context.enumerateInstanceLayerProperties();
		auto unsupportedLayerIt = std::ranges::find_if(
			requiredLayers,
			[&layerProperties](const auto& a_requiredLayer)
			{
				return std::ranges::none_of(
					layerProperties,
					[a_requiredLayer](const auto& a_layerProperty)
					{
						return strcmp(a_layerProperty.layerName, a_requiredLayer) == 0;
					}
				);
			}
		);
		if (unsupportedLayerIt != requiredLayers.end())
		{
			throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
		}

		// Get the required extensions.
		auto requiredExtensions = GetRequiredInstanceExtensions();

		// Check if the required extensions are supported by the Vulkan implementation.
		auto extensionProperties = context.enumerateInstanceExtensionProperties();
		auto unsupportedPropertyIt = std::ranges::find_if(
			requiredExtensions,
			[&extensionProperties](const auto& a_requiredExtension)
			{
				return std::ranges::none_of(
					extensionProperties,
					[a_requiredExtension](const auto& a_extensionProperty)
					{
						return strcmp(a_extensionProperty.extensionName, a_requiredExtension) == 0;
					}
				);
			}
		);
		if (unsupportedPropertyIt != requiredExtensions.end())
		{
			throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
		}

		vk::InstanceCreateInfo createInfo {
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = (uint32) requiredLayers.size(),
			.ppEnabledLayerNames = requiredLayers.data(),
			.enabledExtensionCount = (uint32) requiredExtensions.size(),
			.ppEnabledExtensionNames = requiredExtensions.data(),
		};

		instance = vk::raii::Instance(context, createInfo);
	}

	void
	VulkanRenderContext::Impl::SetupDebugMessenger()
	{
		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		);
		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
			| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
			| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
		);

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT {
			.messageSeverity = severityFlags,
			.messageType = messageTypeFlags,
			.pfnUserCallback = DebugCallback,
		};
		debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	void
	VulkanRenderContext::Impl::CreateSurface()
	{
		VkSurfaceKHR cSurface;
		auto glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindowHandle());
		if (glfwCreateWindowSurface(*instance, glfwWindow, nullptr, &cSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		surface = vk::raii::SurfaceKHR(instance, cSurface);
	}

	void
	VulkanRenderContext::Impl::PickPhysicalDevice()
	{
		auto physicalDevices = instance.enumeratePhysicalDevices();
		const auto iter = std::ranges::find_if(
			physicalDevices,
			[&](const auto& a_physicalDevice)
			{
				return IsDeviceSuitable(a_physicalDevice);
			}
		);
		if (iter == physicalDevices.end())
		{
			throw std::runtime_error("failed to find a suitable GPU with Vulkan support!");
		}
		physicalDevice = *iter;
	}

	void
	VulkanRenderContext::Impl::CreateLogicalDevice()
	{
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports both graphics and present
		uint32 queueIndex = ~0u;
		for (uint32 qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			    physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
			{
				// found a queue family that supports both graphics and present
				queueIndex = qfpIndex;
				break;
			}
		}
		if (queueIndex == ~0u)
		{
			throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
		}

		// Create a chain of feature structures
		vk::StructureChain featureChain = {
			vk::PhysicalDeviceFeatures2(),
			vk::PhysicalDeviceVulkan11Features { .shaderDrawParameters = true },
			vk::PhysicalDeviceVulkan13Features { .dynamicRendering = true },
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT { .extendedDynamicState = true }
		};

		float queuePriority = 0.5f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
			.queueFamilyIndex = queueIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority,
		};

		vk::DeviceCreateInfo deviceCreateInfo {
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledExtensionCount = (uint32) REQUIRED_DEVICE_EXTENSION.size(),
			.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSION.data(),
		};

		device = vk::raii::Device(physicalDevice, deviceCreateInfo);
		graphicsQueue = vk::raii::Queue(device, queueIndex, 0);
	}

	void
	VulkanRenderContext::Impl::CreateSwapChain()
	{
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindowHandle());
		int width, height;
		glfwGetFramebufferSize(glfwWindow, &width, &height);

		vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
		swapChainExtent = ChooseSwapExtent(surfaceCapabilities, { (uint) width, (uint) height });
		uint32_t minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);

		std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
		swapChainSurfaceFormat = ChooseSwapSurfaceFormat(availableFormats);

		std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(availablePresentModes);

		vk::SwapchainCreateInfoKHR swapChainCreateInfo {
			.surface = surface,
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
		swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
		swapChainImages = swapChain.getImages();
	}

	void
	VulkanRenderContext::Impl::CreateSwapChainImageViews()
	{
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
			swapChainImageViews.emplace_back(device, imageViewCreateInfo);
		}
	}
}

namespace
{
	vk::Bool32
	DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT a_severity,
		vk::DebugUtilsMessageTypeFlagsEXT a_type,
		const vk::DebugUtilsMessengerCallbackDataEXT* a_pCallbackData,
		void* a_pUserData
	)
	{
		OYL_UNUSED(a_pUserData);

		constexpr char message[] = "Validation Layer [{}]: {}";

		switch (a_severity)
		{
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			{
				OYL_LOG_DEBUG(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			{
				OYL_LOG_INFO(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			{
				OYL_LOG_WARNING(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			{
				OYL_LOG_ERROR(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			default:
				break;
		}

		return vk::False;
	}

	std::vector<const char*>
	GetRequiredInstanceExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if constexpr (ENABLE_VALIDATION_LAYERS)
		{
			extensions.push_back(vk::EXTDebugUtilsExtensionName);
		}
		return extensions;
	}

	bool
	IsDeviceSuitable(const vk::raii::PhysicalDevice& a_physicalDevice)
	{
		// Check if the physicalDevice supports the Vulkan 1.3 API version
		bool supportsVulkan1_3 = a_physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

		// Check if any of the queue families support graphics operations
		auto queueFamilies = a_physicalDevice.getQueueFamilyProperties();
		bool supportsGraphics = std::ranges::any_of(
			queueFamilies,
			[](const auto& a_qfp)
			{
				return !!(a_qfp.queueFlags & vk::QueueFlagBits::eGraphics);
			}
		);

		// Check if all required physicalDevice extensions are available
		auto availableDeviceExtensions = a_physicalDevice.enumerateDeviceExtensionProperties();
		bool supportsAllRequiredExtensions =
			std::ranges::all_of(
				REQUIRED_DEVICE_EXTENSION,
				[&availableDeviceExtensions](const auto& a_requiredDeviceExtension)
				{
					return std::ranges::any_of(
						availableDeviceExtensions,
						[a_requiredDeviceExtension](const auto& a_availableDeviceExtension)
						{
							return strcmp(a_availableDeviceExtension.extensionName, a_requiredDeviceExtension) == 0;
						}
					);
				}
			);

		// Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
		auto features = a_physicalDevice.getFeatures2<vk::PhysicalDeviceFeatures2,
		                                              vk::PhysicalDeviceVulkan11Features,
		                                              vk::PhysicalDeviceVulkan13Features,
		                                              vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		bool supportsRequiredFeatures = features.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
		                                features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		                                features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

		// Return true if the physicalDevice meets all the criteria
		return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
	}

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
