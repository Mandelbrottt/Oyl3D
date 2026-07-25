#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Core/Logging/Logging.h"

#include "Rendering/RenderEngine.h"
#include "Rendering/Glfw/GlfwWindow.h"
#include "Rendering/Vulkan/VulkanCommandBuffer.h"
#include "Rendering/Vulkan/VulkanCommandPool.h"

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

		// TEMPORARY:
		VertexBuffer vertexBuffer;

		CommandPool commandPool;
		std::vector<CommandBuffer> commandBuffers;

		std::vector<Semaphore> presentCompleteSemaphores;
		std::vector<Semaphore> renderFinishedSemaphores;
		std::vector<Fence> inFlightFences;

		uint32 frameIndex = 0;

		void
		CreateInstance();
		void
		SetupDebugMessenger();
		void
		CreateSyncObjects();
		void
		RecordCommandBuffer();
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

		m_impl->device = Device(
			{
				.instance = m_impl->instance,
				.window = m_impl->window,
				.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
				.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
			}
		);

		m_impl->swapChain = SwapChain(
			{
				.window = m_impl->window,
				.device = &m_impl->device,
			}
		);

		m_impl->commandPool = CommandPool({ .device = m_impl->device });

		m_impl->commandBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_impl->commandBuffers.emplace_back(
				CommandBuffer::CreateParams {
					.device = m_impl->device,
					.commandPool = m_impl->commandPool,
				}
			);
		}

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
			m_impl->shader = RenderEngine::CreateShader(
				{
					.language = SL_Hlsl,
					.source = ShaderOptions::SO_File,
					.filepath = "G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl"
				}
			);
		}

		if (m_impl->shader->IsDirty())
		{
			m_impl->shader->Load();
		}

		if (m_impl->shader->IsDeviceDirty())
		{
			m_impl->shader->DeviceLoad(m_impl->device);
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
			m_impl->vertexBuffer->DeviceLoad(m_impl->device);
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

		m_impl->vertexBuffer->DeviceUnload();
		m_impl->vertexBuffer->Unload();

		m_impl->shader->DeviceUnload();
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

	const SwapChain*
	RenderContext::GetSwapChain() const
	{
		return &m_impl->swapChain;
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
	RenderContext::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.empty() && presentCompleteSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChain.GetVkImages().size(); i++)
		{
			renderFinishedSemaphores.emplace_back(device);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(device);
			inFlightFences.emplace_back(device);
		}
	}

	void
	RenderContext::Impl::RecordCommandBuffer()
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = commandBuffers[frameIndex];

		commandBuffer.Begin();
		commandBuffer.BeginRendering(swapChain);

		Vector2u extent = Vector2u(
			swapChain.GetVkExtent().width,
			swapChain.GetVkExtent().height
		);
		commandBuffer.SetViewport(Vector2i::Zero(), extent);
		commandBuffer.SetScissor(Vector2i::Zero(), extent);

		commandBuffer.BindShader(*shader);
		commandBuffer.BindVertexBuffer(*vertexBuffer);

		commandBuffer.EndRendering(swapChain);
		commandBuffer.End();
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

		auto fenceResult = vkDevice.waitForFences(*drawFence.GetVkFence(), vk::True, UINT64_MAX);
		if (fenceResult != vk::Result::eSuccess)
			throw std::runtime_error("Failed to wait for fence!");

		bool success = swapChain.AcquireNextImage(presentCompleteSemaphore, nullptr);
		if (!success)
		{
			swapChain.Recreate();
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		vkDevice.resetFences(*drawFence.GetVkFence());

		RecordCommandBuffer();
		//graphicsQueue.waitIdle();

		auto& renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];

		{
			OYL_PROFILE_SCOPE("graphicsQueue.submit");

			auto& commandBuffer = commandBuffers[frameIndex].GetVkCommandBuffer();

			vk::PipelineStageFlags waitDestinationStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			const vk::SubmitInfo submitInfo {
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &*presentCompleteSemaphore.GetVkSemaphore(),
				.pWaitDstStageMask = &waitDestinationStageMask,
				.commandBufferCount = 1,
				.pCommandBuffers = &*commandBuffer,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores = &*renderFinishedSemaphore.GetVkSemaphore()
			};
			device.GetVkGraphicsQueue().submit(submitInfo, *drawFence.GetVkFence());
		}
		{
			OYL_PROFILE_SCOPE("graphicsQueue.presentKHR");
			const vk::PresentInfoKHR presentInfoKHR {
				.waitSemaphoreCount = 1,
				.pWaitSemaphores = &*renderFinishedSemaphore.GetVkSemaphore(),
				.swapchainCount = 1,
				.pSwapchains = &*vkSwapChain,
				.pImageIndices = &imageIndex
			};

			auto result = device.GetVkGraphicsQueue().presentKHR(presentInfoKHR);
			if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
				swapChain.Recreate();
			else
			{
				OYL_ASSERT(result == vk::Result::eSuccess);
			}
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
