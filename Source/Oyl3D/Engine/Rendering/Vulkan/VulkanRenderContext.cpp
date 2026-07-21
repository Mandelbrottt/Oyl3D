#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <atlcomcli.h>

#include <Core/Resources/ResourceManager.h>

#include <GLFW/glfw3.h>

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"
#include "VulkanVertexBuffer.h"

#include "Core/Logging/Logging.h"

#include "Rendering/RenderEngine.h"
#include "Rendering/Window.h"

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

namespace Oyl::Rendering::Vulkan
{
	struct RenderContext::Impl
	{
		Window* window;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		vk::raii::SurfaceKHR surface = nullptr;

		Device device;

		//vk::raii::PhysicalDevice physicalDevice = nullptr;

		//uint32 queueIndex;
		//vk::raii::Device device = nullptr;
		//vk::raii::Queue graphicsQueue = nullptr;

		vk::raii::SwapchainKHR swapChain = nullptr;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::raii::ImageView> swapChainImageViews;
		vk::SurfaceFormatKHR swapChainSurfaceFormat;
		vk::Extent2D swapChainExtent;

		ShaderCompiler shaderCompiler;
		Shader shader;

		vk::raii::CommandPool commandPool = nullptr;

		VertexBuffer vertexBuffer;

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
		CreateSwapChain();
		void
		CreateSwapChainImageViews();
		void
		CleanupSwapChain();
		void
		RecreateSwapChain();
		void
		CreateGraphicsPipeline();
		void
		CreateCommandPool();
		void
		CreateVertexBuffer();
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

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const RenderContextParams& a_params) noexcept
		: Rendering::RenderContext(a_params),
		  m_impl(nullptr)
	{
		RenderContext::Init(a_params);
	}

	RenderContext::RenderContext(RenderContext&& a_other) noexcept
		: Rendering::RenderContext(std::move(a_other)),
		  m_impl(nullptr)
	{
		m_impl.swap(a_other.m_impl);
	}

	RenderContext&
	RenderContext::operator=(RenderContext&& a_other) noexcept
	{
		Rendering::RenderContext::operator=(std::move(a_other));
		new(this) RenderContext(std::move(a_other));
		return *this;
	}

	RenderContext::~RenderContext()
	{
		RenderContext::Destroy();
	}

	void
	RenderContext::Init(const RenderContextParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			m_impl->SetupDebugMessenger();
		m_impl->CreateSurface();

		m_impl->device = Device({
			.instance = m_impl->instance,
			.surface = m_impl->surface,
			.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
			.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
		});

		m_impl->CreateSwapChain();
		m_impl->CreateSwapChainImageViews();
		m_impl->CreateGraphicsPipeline();
		m_impl->CreateCommandPool();
		m_impl->CreateVertexBuffer();
		m_impl->CreateCommandBuffers();
		m_impl->CreateSyncObjects();
	}

	void
	RenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;

		if (m_impl->shader->IsDirty())
		{
			m_impl->shader->Load();
		}

		if (m_impl->shader->IsDeviceDirty())
		{
			ShaderResource::DeviceLoadParams params {
				.device = m_impl->device,
				.format = m_impl->swapChainSurfaceFormat.format
			};
			m_impl->shader->DeviceLoad(&params);
		}

		if (m_impl->vertexBuffer->IsDirty())
		{
			m_impl->vertexBuffer->Load();
		}

		if (m_impl->vertexBuffer->IsDeviceDirty())
		{
			VertexBufferResource::DeviceLoadParams params {
				.device = m_impl->device.GetVkDevice(),
				.physicalDevice = m_impl->device.GetVkPhysicalDevice(),
				.commandPool = m_impl->commandPool,
				.queue = m_impl->device.GetVkGraphicsQueue()
			};
			m_impl->vertexBuffer->DeviceLoad(&params);
		}

		m_impl->DrawFrame();
	}

	void
	RenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		m_impl->device.GetVkDevice().waitIdle();

		m_impl->vertexBuffer->DeviceUnload(nullptr);
		m_impl->vertexBuffer->Unload();

		m_impl->shader->DeviceUnload(nullptr);
		m_impl->shader->Unload();

		m_impl->CleanupSwapChain();

		*m_impl = {};
	}

	void
	RenderContext::Resize(Vector2i /*a_size*/)
	{
		m_impl->RecreateSwapChain();
	}

	void
	RenderContext::Impl::CreateInstance()
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
	RenderContext::Impl::SetupDebugMessenger()
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
	RenderContext::Impl::CreateSurface()
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
	RenderContext::Impl::CreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindowHandle());
		int width, height;
		glfwGetFramebufferSize(glfwWindow, &width, &height);

		const auto& physicalDevice = device.GetVkPhysicalDevice();
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
		swapChain = vk::raii::SwapchainKHR(device.GetVkDevice(), swapChainCreateInfo);
		swapChainImages = swapChain.getImages();
	}

	void
	RenderContext::Impl::CreateSwapChainImageViews()
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
			swapChainImageViews.emplace_back(device.GetVkDevice(), imageViewCreateInfo);
		}
	}

	void
	RenderContext::Impl::CleanupSwapChain()
	{
		swapChainImageViews.clear();
		swapChain = nullptr;
	}

	void
	RenderContext::Impl::RecreateSwapChain()
	{
		device.GetVkDevice().waitIdle();

		CleanupSwapChain();

		int width = 0, height = 0;
		auto glfwWindow = static_cast<GLFWwindow*>(window->GetNativeWindowHandle());
		glfwGetFramebufferSize(glfwWindow, &width, &height);
		if (width == 0 || height == 0)
		{
			return;
		}

		CreateSwapChain();
		CreateSwapChainImageViews();
	}

	void
	RenderContext::Impl::CreateGraphicsPipeline()
	{
		OYL_PROFILE_FUNCTION();

		auto options = ShaderOptions {
			.language = SL_Hlsl,
			.source = ShaderOptions::SO_File,
			.filepath = "G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl"
		};
		shader = RenderEngine::CreateShader(options);
	}

	void
	RenderContext::Impl::CreateCommandPool()
	{
		OYL_PROFILE_FUNCTION();

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = device.GetVkGraphicsQueueIndex(),
		};

		commandPool = vk::raii::CommandPool(device.GetVkDevice(), poolInfo);
	}

	const std::vector g_vertices {
		Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
		Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
		Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
		Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
	};

	const std::vector<uint16> g_indices { 0, 1, 2, 2, 3, 0 };

	void
	RenderContext::Impl::CreateVertexBuffer()
	{
		auto verticesBuffer = reinterpret_cast<const byte*>(g_vertices.data());
		auto indicesBuffer = reinterpret_cast<const byte*>(g_indices.data());

		vertexBuffer = RenderEngine::CreateVertexBuffer({
			.vertexData = verticesBuffer,
			.vertexLength = g_vertices.size() * sizeof(decltype(g_vertices)::value_type),
			.indexData = indicesBuffer,
			.indexLength = g_indices.size() * sizeof(decltype(g_indices)::value_type)
		});
	}

	void
	RenderContext::Impl::CreateCommandBuffers()
	{
		OYL_PROFILE_FUNCTION();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = MAX_FRAMES_IN_FLIGHT
		};

		commandBuffers = vk::raii::CommandBuffers(device.GetVkDevice(), allocInfo);
	}

	void
	RenderContext::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.empty() && presentCompleteSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			renderFinishedSemaphores.emplace_back(device.GetVkDevice(), vk::SemaphoreCreateInfo {});
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(
				device.GetVkDevice(),
				vk::SemaphoreCreateInfo {}
			);
			inFlightFences.emplace_back(
				device.GetVkDevice(),
				vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled }
			);
		}
	}

	void
	RenderContext::Impl::RecordCommandBuffer(uint32 a_imageIndex)
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

		auto* vkShader = shader.Get();
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkShader->GetPipeline());
		if (vertexBuffer->HasIndexData())
		{
			commandBuffer.bindIndexBuffer(*vertexBuffer->GetVkBuffer(), 0, vk::IndexType::eUint16);
			commandBuffer.bindVertexBuffers(0, *vertexBuffer->GetVkBuffer(), { vertexBuffer->GetVertexDataOffset() });
			commandBuffer.drawIndexed(static_cast<uint32>(g_indices.size()), 1, 0, 0, 0);
		} else
		{
			commandBuffer.bindVertexBuffers(0, *vertexBuffer->GetVkBuffer(), { vertexBuffer->GetVertexDataOffset() });
			commandBuffer.draw(static_cast<uint32>(g_vertices.size()), 1, 0, 0);
		}

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
	RenderContext::Impl::TransitionImageLayout(uint32 a_imageIndex, vk::ImageLayout a_oldLayout, vk::ImageLayout a_new_layout, vk::AccessFlags2 a_srcAccessMask, vk::AccessFlags2 a_dstAccessMask, vk::PipelineStageFlags2 a_srcStageMask, vk::PipelineStageFlags2 a_dstStageMask)
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
	RenderContext::Impl::DrawFrame()
	{
		OYL_PROFILE_FUNCTION();

		if (swapChain == nullptr)
			return;

		auto& drawFence = inFlightFences[frameIndex];
		auto& presentCompleteSemaphore = presentCompleteSemaphores[frameIndex];
		auto& commandBuffer = commandBuffers[frameIndex];

		auto fenceResult = device.GetVkDevice().waitForFences(*drawFence, vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to wait for fence!");

		auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			RecreateSwapChain();
			return;
		}
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			OYL_ASSERT(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		// Only reset fences if we are going to submit work to the GPU
		device.GetVkDevice().resetFences(*drawFence);

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
			device.GetVkGraphicsQueue().submit(submitInfo, *drawFence);
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

			result = device.GetVkGraphicsQueue().presentKHR(presentInfoKHR);
			if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
				RecreateSwapChain();
			else
				OYL_ASSERT(result == vk::Result::eSuccess);
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
