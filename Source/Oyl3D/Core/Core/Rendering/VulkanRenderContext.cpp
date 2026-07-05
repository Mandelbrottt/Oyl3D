#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "Core/Logging/Logging.h"

static const std::vector VALIDATION_LAYERS {
	"VK_LAYER_KHRONOS_validation",
};

constexpr bool ENABLE_VALIDATION_LAYERS =
#if defined(OYL_DISTRIBUTION)
	false;
#else
	true;
#endif

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
}

namespace Oyl::Rendering
{
	struct VulkanRenderContext::Impl
	{
		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;

		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	};

	VulkanRenderContext::VulkanRenderContext() noexcept
		: m_impl(nullptr) {}

	VulkanRenderContext::VulkanRenderContext(const RenderContextParams& a_params) noexcept
		: m_impl(new Impl)
	{
		VulkanRenderContext::Init(a_params);
	}

	VulkanRenderContext::VulkanRenderContext(VulkanRenderContext&& a_other) noexcept
		: RenderContext(std::move(a_other)),
		  m_impl(nullptr)
	{
		std::swap(m_impl, a_other.m_impl);
	}

	VulkanRenderContext&
	VulkanRenderContext::operator=(VulkanRenderContext&& a_other) noexcept
	{
		RenderContext::operator=(std::move(a_other));
		new(this) VulkanRenderContext(std::move(a_other));
		return *this;
	}

	void
	VulkanRenderContext::Init(const RenderContextParams& a_params)
	{
		OYL_UNUSED(a_params);

		CreateInstance();
		SetupDebugMessenger();
	}

	void
	VulkanRenderContext::Update() {}

	void
	VulkanRenderContext::Destroy() {}

	void
	VulkanRenderContext::CreateInstance()
	{
		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName = "Oyl3D";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Oyl3D";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = vk::ApiVersion14;

		// Get the required layers
		std::vector<const char*> requiredLayers;
		if (ENABLE_VALIDATION_LAYERS)
		{
			requiredLayers.assign(VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
		}

		// Check if the required layers are supported by the Vulkan implementation.
		auto layerProperties = m_impl->context.enumerateInstanceLayerProperties();
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
		auto extensionProperties = m_impl->context.enumerateInstanceExtensionProperties();
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

		vk::InstanceCreateInfo createInfo;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = (uint32) requiredLayers.size();
		createInfo.ppEnabledLayerNames = requiredLayers.data();
		createInfo.enabledExtensionCount = (uint32) requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		m_impl->instance = vk::raii::Instance(m_impl->context, createInfo);
	}

	void
	VulkanRenderContext::SetupDebugMessenger()
	{
		if constexpr (!ENABLE_VALIDATION_LAYERS)
			return;

		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
			| vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
		);
		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
			| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
			| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
		);

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
		debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
		debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
		debugUtilsMessengerCreateInfoEXT.pfnUserCallback = DebugCallback;
		m_impl->debugMessenger = m_impl->instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	std::vector<const char*>
	VulkanRenderContext::GetRequiredInstanceExtensions() const
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

		switch (a_severity)
		{
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			{
				OYL_LOG_DEBUG("Validation Layer [{}]: {}", to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			{
				OYL_LOG_INFO("Validation Layer [{}]: {}", to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			{
				OYL_LOG_WARNING("Validation Layer [{}]: {}", to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			{
				OYL_LOG_ERROR("Validation Layer [{}]: {}", to_string(a_type).data(), a_pCallbackData->pMessage);
				break;
			}
			default:
				break;
		}

		return vk::False;
	}
}
