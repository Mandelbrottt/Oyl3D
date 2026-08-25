#include "VulkanRenderEngine.h"

#include <GLFW/glfw3.h>

#include "VulkanRenderContext.h"
#include "VulkanRenderContext.h"
#include "VulkanShaderCompiler.h"

namespace
{
	const std::vector VALIDATION_LAYERS {
		"VK_LAYER_KHRONOS_validation",
	};

	constexpr bool ENABLE_VALIDATION_LAYERS =
	#if defined(OYL_DISTRIBUTION)
		false;
	#else
		true;
	#endif

	std::vector<const char*>
	GetRequiredInstanceExtensions();

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

namespace Oyl::Internal
{
	VulkanRenderEngineInstance::VulkanRenderEngineInstance(CreateParams a_params)
	{
		CreateVkInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			CreateVkDebugMessenger();

		m_shaderCompiler = UniquePtr<Rendering::VulkanShaderCompiler>::Create();
		m_renderContext = UniquePtr<Rendering::VulkanRenderContext>::Create(Rendering::VulkanRenderContext::CreateParams {
			.window = a_params.window,
			.vkInstance = m_vkInstance,
		});
	}

	VulkanRenderEngineInstance::~VulkanRenderEngineInstance()
	{
		// Destroy manually to ensure proper destruction order
		m_renderContext->Destroy();
	}

	Rendering::PresentTargetHandle
	VulkanRenderEngineInstance::CreatePresentTarget(const Rendering::PresentTarget::CreateParams& a_params) const
	{
		return Rendering::VulkanPresentTargetHandle(
			UniquePtr<Rendering::VulkanPresentTarget>::Create(Rendering::VulkanPresentTarget::CreateParams {
				.window = a_params.window,
				.vkInstance = m_vkInstance
			})
		);
	}

	void
	VulkanRenderEngineInstance::CreateVkInstance()
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
		auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();
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
		auto extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
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

		m_vkInstance = vk::raii::Instance(m_vkContext, createInfo);
	}

	void
	VulkanRenderEngineInstance::CreateVkDebugMessenger()
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
