#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <atlcomcli.h>

#include <Core/Application/SharedLibrary.h>
#include <Core/Resources/ResourceManager.h>

#include <dxc/dxcapi.h>

#include <GLFW/glfw3.h>

#include "Core/Logging/Logging.h"

#include "Rendering/Shaders/VulkanShader.h"
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

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering::Internal
{
	struct VulkanRenderContext::Impl
	{
		Window* window;
		Oyl::Internal::ResourceManager* resourceManager;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		vk::raii::SurfaceKHR surface = nullptr;

		vk::raii::PhysicalDevice physicalDevice = nullptr;

		uint32 queueIndex;
		vk::raii::Device device = nullptr;
		vk::raii::Queue graphicsQueue = nullptr;

		vk::raii::SwapchainKHR swapChain = nullptr;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::raii::ImageView> swapChainImageViews;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		vk::Extent2D swapChainExtent;

		vk::raii::ShaderModule vsShaderModule = nullptr;
		vk::raii::ShaderModule fsShaderModule = nullptr;

		//vk::raii::PipelineLayout pipelineLayout = nullptr;
		//vk::raii::Pipeline graphicsPipeline = nullptr;
		VulkanShader shader;

		vk::raii::CommandPool commandPool = nullptr;

		std::vector<vk::raii::CommandBuffer> commandBuffers;

		std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
		std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
		std::vector<vk::raii::Fence> inFlightFences;

		uint32 frameIndex = 0;

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
		void
		CreateGraphicsPipeline();
		void
		CreateCommandPool();
		void
		CreateCommandBuffers();
		void
		CreateSyncObjects();
		void
		RecordCommandBuffer(uint32 a_imageIndex);
		void
		TransitionImageLayout(
			uint32 a_imageIndex,
			vk::ImageLayout a_oldLayout,
			vk::ImageLayout a_new_layout,
			vk::AccessFlags2 a_srcAccessMask,
			vk::AccessFlags2 a_dstAccessMask,
			vk::PipelineStageFlags2 a_srcStageMask,
			vk::PipelineStageFlags2 a_dstStageMask
		);
		void
		DrawFrame();
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
		m_impl->resourceManager = a_params.resourceManager;

		m_impl->CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			m_impl->SetupDebugMessenger();
		m_impl->CreateSurface();
		m_impl->PickPhysicalDevice();
		m_impl->CreateLogicalDevice();
		m_impl->CreateSwapChain();
		m_impl->CreateSwapChainImageViews();
		m_impl->CreateGraphicsPipeline();
		m_impl->CreateCommandPool();
		m_impl->CreateCommandBuffers();
		m_impl->CreateSyncObjects();
	}

	void
	VulkanRenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;

		m_impl->DrawFrame();
	}

	void
	VulkanRenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		m_impl->device.waitIdle();

		m_impl->shader->Unload();

		*m_impl = {};
	}

	void
	VulkanRenderContext::Impl::CreateInstance()
	{
		OYL_PROFILE_FUNCTION();

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
		OYL_PROFILE_FUNCTION();

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
		OYL_PROFILE_FUNCTION();

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
		OYL_PROFILE_FUNCTION();

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
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports both graphics and present
		queueIndex = ~0u;
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
			vk::PhysicalDeviceVulkan13Features { .synchronization2 = true, .dynamicRendering = true },
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
		OYL_PROFILE_FUNCTION();

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
			swapChainImageViews.emplace_back(device, imageViewCreateInfo);
		}
	}

	void
	VulkanRenderContext::Impl::CreateGraphicsPipeline()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(resourceManager);

		shader = resourceManager->CreateHandle<VulkanShaderResource>();
		shader->SetFilePath("G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Core/Shaders/shader.hlsl");
		shader->Load();
	}

	void
	VulkanRenderContext::Impl::CreateCommandPool()
	{
		OYL_PROFILE_FUNCTION();

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = queueIndex,
		};

		commandPool = vk::raii::CommandPool(device, poolInfo);
	}

	void
	VulkanRenderContext::Impl::CreateCommandBuffers()
	{
		OYL_PROFILE_FUNCTION();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = MAX_FRAMES_IN_FLIGHT
		};

		commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
	}

	void
	VulkanRenderContext::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.empty() && presentCompleteSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo {});
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo {});
			inFlightFences.emplace_back(device, vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled });
		}
	}

	void
	VulkanRenderContext::Impl::RecordCommandBuffer(uint32 a_imageIndex)
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = commandBuffers[frameIndex];

		commandBuffer.begin({});

		TransitionImageLayout(
			a_imageIndex,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
		vk::RenderingAttachmentInfo attachmentInfo = {
			.imageView = swapChainImageViews[a_imageIndex],
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = clearColor
		};

		vk::RenderingInfo renderingInfo = {
			.renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentInfo
		};

		commandBuffer.beginRendering(renderingInfo);

		if (shader->IsDirty())
		{
			VulkanShaderCompileInput input {
				.device = device,
				.format = swapChainSurfaceFormat.format
			};
			shader->Compile(input);
		}

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shader->GetPipeline());
		auto viewport = vk::Viewport {
			0.0f,
			0.0f,
			static_cast<float>(swapChainExtent.width),
			static_cast<float>(swapChainExtent.height),
			0.0f,
			1.0f
		};
		auto scissor = vk::Rect2D {
			vk::Offset2D(0, 0),
			swapChainExtent
		};
		commandBuffer.setViewport(0, viewport);
		commandBuffer.setScissor(0, scissor);
		commandBuffer.draw(3, 1, 0, 0);
		commandBuffer.endRendering();
		TransitionImageLayout(
			a_imageIndex,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe
		);
		commandBuffer.end();
	}

	void
	VulkanRenderContext::Impl::TransitionImageLayout(uint32 a_imageIndex, vk::ImageLayout a_oldLayout, vk::ImageLayout a_new_layout, vk::AccessFlags2 a_srcAccessMask, vk::AccessFlags2 a_dstAccessMask, vk::PipelineStageFlags2 a_srcStageMask, vk::PipelineStageFlags2 a_dstStageMask)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = a_srcStageMask,
			.srcAccessMask = a_srcAccessMask,
			.dstStageMask = a_dstStageMask,
			.dstAccessMask = a_dstAccessMask,
			.oldLayout = a_oldLayout,
			.newLayout = a_new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = swapChainImages[a_imageIndex],
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		vk::DependencyInfo dependency_info = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		commandBuffers[frameIndex].pipelineBarrier2(dependency_info);
	}

	void
	VulkanRenderContext::Impl::DrawFrame()
	{
		OYL_PROFILE_FUNCTION();

		auto& drawFence = inFlightFences[frameIndex];
		auto& presentCompleteSemaphore = presentCompleteSemaphores[frameIndex];
		auto& commandBuffer = commandBuffers[frameIndex];

		auto fenceResult = device.waitForFences(*drawFence, vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to wait for fence!");
		device.resetFences(*drawFence);

		auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);
		RecordCommandBuffer(imageIndex);
		//graphicsQueue.waitIdle();

		auto& renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];

		{
			OYL_PROFILE_SCOPE("graphicsQueue.submit");
			vk::PipelineStageFlags waitDestinationStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			const vk::SubmitInfo submitInfo {
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &*presentCompleteSemaphore,
				.pWaitDstStageMask = &waitDestinationStageMask,
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandBuffer,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &*renderFinishedSemaphore
			};
			graphicsQueue.submit(submitInfo, *drawFence);
		}
		{
			OYL_PROFILE_SCOPE("graphicsQueue.presentKHR");
			const vk::PresentInfoKHR presentInfoKHR {
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &*renderFinishedSemaphore,
				.swapchainCount = 1,
				.pSwapchains = &*swapChain,
				.pImageIndices = &imageIndex
			};

			result = graphicsQueue.presentKHR(presentInfoKHR);
		}

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
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
