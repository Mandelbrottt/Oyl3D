#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Core/Logging/Logging.h"

#include "Rendering/RenderEngine.h"

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
}

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering::Vulkan
{
	struct RenderContext::Impl
	{
		const Window* window;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		Device device;

		SwapChain swapChain;

		// TEMPORARY:
		Shader shader;

		vk::raii::CommandPool commandPool = nullptr;

		// TEMPORARY:
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

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const CreateParams& a_params) noexcept
		: m_impl(nullptr)
	{
		Init(a_params);
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
	RenderContext::Init(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			m_impl->SetupDebugMessenger();

		m_impl->device = Device({
			.instance = m_impl->instance,
			.window = m_impl->window,
			.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
			.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
		});

		m_impl->swapChain = SwapChain({
			.window = m_impl->window,
			.device = &m_impl->device,
		});

		m_impl->CreateCommandPool();

		m_impl->CreateCommandBuffers();
		m_impl->CreateSyncObjects();
	}

	void
	RenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;

		if (!m_impl->shader)
		{
			m_impl->shader = RenderEngine::CreateShader({
				.language = SL_Hlsl,
				.source = ShaderOptions::SO_File,
				.filepath = "G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl"
			});
		}

		if (m_impl->shader->IsDirty())
		{
			m_impl->shader->Load();
		}

		if (m_impl->shader->IsDeviceDirty())
		{
			ShaderResource::DeviceLoadParams params {
				.device = m_impl->device,
				.format = m_impl->swapChain.GetVkSurfaceFormat().format
			};
			m_impl->shader->DeviceLoad(&params);
		}

		if (!m_impl->vertexBuffer)
		{
			const std::vector vertices {
				Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
				Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
				Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
				Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
			};

			const std::vector<uint16> indices { 0, 1, 2, 2, 3, 0 };

			auto verticesBuffer = reinterpret_cast<const byte*>(vertices.data());
			auto indicesBuffer = reinterpret_cast<const byte*>(indices.data());

			m_impl->vertexBuffer = RenderEngine::CreateVertexBuffer(
				{
					.vertexData = verticesBuffer,
					.vertexLength = vertices.size() * sizeof(decltype(vertices)::value_type),
					.vertexStride = sizeof(decltype(vertices)::value_type),
					.indexData = indicesBuffer,
					.indexLength = indices.size() * sizeof(decltype(indices)::value_type),
				}
			);
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

		m_impl->swapChain.Destroy();
		m_impl->device.Destroy();

		*m_impl = {};
	}

	void
	RenderContext::Resize(Vector2i /*a_size*/)
	{
		// No need to pass in a_size - we get the size from the window directly
		m_impl->swapChain.Recreate();
	}

	const Device*
	RenderContext::GetDevice() const
	{
		return &m_impl->device;
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
	RenderContext::Impl::CreateCommandPool()
	{
		OYL_PROFILE_FUNCTION();

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = device.GetVkGraphicsQueueIndex(),
		};

		commandPool = vk::raii::CommandPool(device.GetVkDevice(), poolInfo);
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

		const auto& vkDevice = device.GetVkDevice();

		for (size_t i = 0; i < swapChain.GetVkImages().size(); i++)
		{
			renderFinishedSemaphores.emplace_back(vkDevice, vk::SemaphoreCreateInfo {});
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(
				vkDevice,
				vk::SemaphoreCreateInfo {}
			);
			inFlightFences.emplace_back(
				vkDevice,
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

		const auto& swapChainImageViews = swapChain.GetVkImageViews();

		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
		vk::RenderingAttachmentInfo attachmentInfo = {
			.imageView = swapChainImageViews[a_imageIndex],
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = clearColor
		};

		const auto& swapChainExtent = swapChain.GetVkExtent();

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
			commandBuffer.drawIndexed(vertexBuffer->GetIndexCount(), 1, 0, 0, 0);
		} else
		{
			commandBuffer.bindVertexBuffers(0, *vertexBuffer->GetVkBuffer(), { vertexBuffer->GetVertexDataOffset() });
			commandBuffer.draw(vertexBuffer->GetVertexCount(), 1, 0, 0);
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

		const auto& swapChainImages = swapChain.GetVkImages();

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

		if (!swapChain)
			return;

		const auto& vkDevice = device.GetVkDevice();
		const auto& vkSwapChain = swapChain.GetVkSwapChain();

		auto& drawFence = inFlightFences[frameIndex];
		auto& presentCompleteSemaphore = presentCompleteSemaphores[frameIndex];
		auto& commandBuffer = commandBuffers[frameIndex];

		auto fenceResult = vkDevice.waitForFences(*drawFence, vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to wait for fence!");

		auto [result, imageIndex] = vkSwapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphore, nullptr);
		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			swapChain.Recreate();
			return;
		}
		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		{
			OYL_ASSERT(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		// Only reset fences if we are going to submit work to the GPU
		vkDevice.resetFences(*drawFence);

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
				.pSwapchains = &*vkSwapChain,
				.pImageIndices = &imageIndex
			};

			result = device.GetVkGraphicsQueue().presentKHR(presentInfoKHR);
			if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
				swapChain.Recreate();
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
}
