#include "VulkanRenderer.h"

#include <Application/Application.h>

#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>

#include <GLFW/glfw3.h>

#include "VulkanPresentTarget.h"

namespace
{
	const Oyl::Array<const char*> VALIDATION_LAYERS = std::vector {
		"VK_LAYER_KHRONOS_validation",
	};

	constexpr bool ENABLE_VALIDATION_LAYERS =
	#if defined(OYL_DISTRIBUTION)
		false;
	#else
		true;
	#endif

	const Oyl::Array<const char*> REQUIRED_DEVICE_EXTENSIONS = std::vector {
		vk::KHRSwapchainExtensionName,
	};

	Oyl::Array<const char*>
	GetRequiredInstanceExtensions();

	extern "C"
	VKAPI_ATTR
	vk::Bool32
	VKAPI_CALL
	DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT a_severity,
		vk::DebugUtilsMessageTypeFlagsEXT a_type,
		const vk::DebugUtilsMessengerCallbackDataEXT* a_pCallbackData,
		void* a_pUserData
	);
}

namespace Oyl
{
	void
	VulkanRenderer::Init()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(!*m_vkInstance);

		CreateVkInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			CreateVkDebugMessenger();
	}

	void
	VulkanRenderer::CreateVkInstance()
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
		Array<const char*> requiredLayers;
		if constexpr (ENABLE_VALIDATION_LAYERS)
		{
			requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
		}

		// Check if the required layers are supported by the Vulkan implementation.
		auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();
		auto unsupportedLayerIt = std::ranges::find_if(
			requiredLayers,
			[&layerProperties](const char* a_requiredLayer)
			{
				return std::ranges::none_of(
					layerProperties,
					[a_requiredLayer](const vk::LayerProperties& a_layerProperty)
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
		auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
		auto unsupportedPropertyIt = std::ranges::find_if(
			requiredExtensions,
			[&extensionProperties](const String& a_requiredExtension)
			{
				return std::ranges::none_of(
					extensionProperties,
					[a_requiredExtension](const vk::ExtensionProperties& a_extensionProperty)
					{
						return a_extensionProperty.extensionName == a_requiredExtension;
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

		m_vkInstance = vk::raii::Instance(m_vkContext, createInfo);
	}

	void
	VulkanRenderer::CreateVkDebugMessenger()
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
		m_debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	PresentTargetHandle
	VulkanRenderer::CreatePresentTarget(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();
		VulkanPresentTargetCreateParams params {
			.vkInstance = &m_vkInstance,
			.window = &a_window
		};
		return MakeSharedPtr<VulkanPresentTarget>(params);
	}

	DeviceHandle
	VulkanRenderer::CreateDevice(IPresentTarget* a_presentTarget)
	{
		OYL_PROFILE_FUNCTION();
		VulkanDeviceCreateParams params {
			.presentTarget = dynamic_cast<VulkanPresentTarget*>(a_presentTarget),
			.commandQueueFlags = CommandQueueFlagBits::Graphics | CommandQueueFlagBits::Transfer,
			.requiredDeviceExtensions = REQUIRED_DEVICE_EXTENSIONS,
		};
		return MakeSharedPtr<VulkanDevice>(m_vkInstance, params);
	}

	SwapChainHandle
	VulkanRenderer::CreateSwapChain(IDevice& a_device, IApplicationWindow& a_window, IPresentTarget& a_presentTarget)
	{
		OYL_PROFILE_FUNCTION();
		VulkanSwapChainCreateParams params {
			.window = a_window,
			.presentTarget = dynamic_cast<VulkanPresentTarget&>(a_presentTarget)
		};
		return MakeSharedPtr<VulkanSwapChain>(dynamic_cast<VulkanDevice&>(a_device), params);
	}
}

namespace
{
	Oyl::Array<const char*>
	GetRequiredInstanceExtensions()
	{
		Oyl::Array<const char*> result;

		const char** extensions = nullptr;
		uint32 count = 0;

		if (Oyl::ModuleManager::GetModulePointer("GlfwModule"))
			extensions = glfwGetRequiredInstanceExtensions(&count);

		if (extensions && count > 0)
		{
			result.reserve(count);
			for (uint32 i = 0; i < count; i++)
				result.Add(extensions[i]);
		}

		if constexpr (ENABLE_VALIDATION_LAYERS)
		{
			result.Add(vk::EXTDebugUtilsExtensionName);
		}
		return result;
	}

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
				Oyl::LogDebug(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			{
				Oyl::LogInfo(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			{
				Oyl::LogWarning(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			{
				Oyl::LogError(message, to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			default:
				break;
		}

		return vk::False;
	}
}
