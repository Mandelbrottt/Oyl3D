#include "VulkanDevice.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "Rendering/Glfw/GlfwWindow.h"

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

namespace Oyl::Rendering::Vulkan
{
	struct Device::Impl
	{
		const Glfw::Window* window;

		std::vector<std::string> requiredDeviceExtensions;

		vk::raii::Context context;
		vk::raii::Instance instance = nullptr;
		vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

		vk::raii::SurfaceKHR surface = nullptr;

		vk::raii::PhysicalDevice physicalDevice = nullptr;
		vk::raii::Device device = nullptr;

		vk::raii::Queue graphicsQueue = nullptr;
		uint32 graphicsQueueIndex = 0;

		void
		CreateInstance();
		void
		CreateDebugMessenger();
		void
		CreateSurface();
		void
		PickPhysicalDevice();
		void
		CreateLogicalDevice();
	};

	Device::Device()
		: m_impl(nullptr) {}

	Device::Device(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->window = static_cast<const Glfw::Window*>(&a_params.window);

		if (a_params.ppRequiredDeviceExtensionsData && a_params.requiredDeviceExtensionsLength > 0)
		{
			m_impl->requiredDeviceExtensions.reserve(a_params.requiredDeviceExtensionsLength);
			m_impl->requiredDeviceExtensions.insert(
				m_impl->requiredDeviceExtensions.end(),
				a_params.ppRequiredDeviceExtensionsData,
				a_params.ppRequiredDeviceExtensionsData + a_params.requiredDeviceExtensionsLength
			);
		}

		m_impl->CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			m_impl->CreateDebugMessenger();
		m_impl->CreateSurface();
		m_impl->PickPhysicalDevice();
		m_impl->CreateLogicalDevice();
	}

	Device::Device(Device&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	Device&
	Device::operator=(Device&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	Device::~Device()
	{
		Device::Destroy();
	}

	void
	Device::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_impl->graphicsQueueIndex = 0;
		m_impl->graphicsQueue = nullptr;
		m_impl->device = nullptr;
		m_impl->physicalDevice = nullptr;
		m_impl->requiredDeviceExtensions.clear();

		m_impl.release();
	}

	bool
	Device::IsValid() const
	{
		return m_impl
		       && *m_impl->device;
	}

	const IWindow*
	Device::GetWindow() const
	{
		return m_impl->window;
	}

	const vk::raii::Device&
	Device::GetVkDevice() const
	{
		return m_impl->device;
	}

	const vk::raii::PhysicalDevice&
	Device::GetVkPhysicalDevice() const
	{
		return m_impl->physicalDevice;
	}

	const vk::raii::SurfaceKHR&
	Device::GetVkSurface() const
	{
		return m_impl->surface;
	}

	const vk::raii::Queue&
	Device::GetVkGraphicsQueue() const
	{
		return m_impl->graphicsQueue;
	}

	uint32
	Device::GetVkGraphicsQueueIndex() const
	{
		return m_impl->graphicsQueueIndex;
	}

	void
	Device::Impl::CreateInstance()
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
	Device::Impl::CreateDebugMessenger()
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
	Device::Impl::CreateSurface()
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

	bool
	IsDeviceSuitable(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		const std::vector<std::string>& a_requiredDeviceExtensions
	)
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
				a_requiredDeviceExtensions,
				[&availableDeviceExtensions](const std::string& a_requiredDeviceExtension)
				{
					return std::ranges::any_of(
						availableDeviceExtensions,
						[a_requiredDeviceExtension](const vk::ExtensionProperties& a_availableDeviceExtension)
						{
							auto compare = strcmp(
								a_availableDeviceExtension.extensionName,
								a_requiredDeviceExtension.c_str()
							);
							return compare == 0;
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

	void
	Device::Impl::PickPhysicalDevice()
	{
		OYL_PROFILE_FUNCTION();

		auto physicalDevices = instance.enumeratePhysicalDevices();
		const auto iter = std::ranges::find_if(
			physicalDevices,
			[&](const vk::raii::PhysicalDevice& a_physicalDevice)
			{
				return IsDeviceSuitable(a_physicalDevice, requiredDeviceExtensions);
			}
		);
		if (iter == physicalDevices.end())
		{
			throw std::runtime_error("failed to find a suitable GPU with Vulkan support!");
		}
		physicalDevice = *iter;
	}

	void
	Device::Impl::CreateLogicalDevice()
	{
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports both graphics and present
		graphicsQueueIndex = ~0u;
		for (uint32 qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			    physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
			{
				// found a queue family that supports both graphics and present
				graphicsQueueIndex = qfpIndex;
				break;
			}
		}
		if (graphicsQueueIndex == ~0u)
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
			.queueFamilyIndex = graphicsQueueIndex,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority,
		};

		// Vulkan needs the extensions array as c-strings
		std::vector<const char*> requiredExtensions(requiredDeviceExtensions.size(), nullptr);
		for (size_t i = 0; i < requiredDeviceExtensions.size(); i++)
		{
			requiredExtensions[i] = requiredDeviceExtensions[i].c_str();
		}

		vk::DeviceCreateInfo deviceCreateInfo {
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &deviceQueueCreateInfo,
			.enabledExtensionCount = (uint32) requiredDeviceExtensions.size(),
			.ppEnabledExtensionNames = requiredExtensions.data(),
		};

		device = vk::raii::Device(physicalDevice, deviceCreateInfo);
		graphicsQueue = vk::raii::Queue(device, graphicsQueueIndex, 0);
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
