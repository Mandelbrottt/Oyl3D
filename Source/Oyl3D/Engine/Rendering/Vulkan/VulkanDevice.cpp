#include "VulkanDevice.h"

#include <vulkan/vulkan_raii.hpp>

#include <GLFW/glfw3.h>

#include "VulkanCommandBuffer.h"
#include "VulkanEnums.h"

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
	DeviceImpl::DeviceImpl(nullptr_t) {}

	Device
	DeviceImpl::Create(const CreateParams& a_params)
	{
		return Device(std::make_unique<DeviceImpl>(DeviceTag(), a_params));
	}

	DeviceImpl::DeviceImpl(DeviceTag, const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		m_window = static_cast<const Glfw::Window*>(&a_params.window);

		if (a_params.ppRequiredDeviceExtensionsData && a_params.requiredDeviceExtensionsLength > 0)
		{
			m_requiredDeviceExtensions.reserve(a_params.requiredDeviceExtensionsLength);
			m_requiredDeviceExtensions.insert(
				m_requiredDeviceExtensions.end(),
				a_params.ppRequiredDeviceExtensionsData,
				a_params.ppRequiredDeviceExtensionsData + a_params.requiredDeviceExtensionsLength
			);
		}

		for (uint32 i = 0; i < sizeof(a_params.commandQueueFlags) * 8; i++)
		{
			auto flag = CommandQueueFlagBits(1 << i);
			if (a_params.commandQueueFlags & flag)
			{
				m_queueFamilyIndices[flag] = 0;
			}
		}

		CreateInstance();
		if constexpr (ENABLE_VALIDATION_LAYERS)
			CreateDebugMessenger();
		CreateSurface();
		PickPhysicalDevice(a_params.commandQueueFlags);
		CreateLogicalDevice();
		CreateCommandQueues();
	}

	DeviceImpl::~DeviceImpl()
	{
		DeviceImpl::Destroy();
	}

	void
	DeviceImpl::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_queues.clear();
		m_queueFamilyIndices.clear();
		m_device = nullptr;
		m_physicalDevice = nullptr;
		m_requiredDeviceExtensions.clear();
	}

	bool
	DeviceImpl::IsValid() const
	{
		return *m_device;
	}

	const IWindow*
	DeviceImpl::GetWindow() const
	{
		return m_window;
	}

	const CommandQueueImpl*
	DeviceImpl::GetCommandQueue(CommandQueueFlagBits a_flag) const
	{
		auto iter = m_queues.find(a_flag);
		if (iter == m_queues.end())
			return nullptr;

		return &iter->second;
	}

	const vk::raii::Device&
	DeviceImpl::GetVkDevice() const
	{
		return m_device;
	}

	const vk::raii::PhysicalDevice&
	DeviceImpl::GetVkPhysicalDevice() const
	{
		return m_physicalDevice;
	}

	const vk::raii::SurfaceKHR&
	DeviceImpl::GetVkSurface() const
	{
		return m_surface;
	}

	void
	DeviceImpl::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_device.waitIdle();
	}

	Rendering::CommandBuffer
	DeviceImpl::CreateCommandBuffer(const CommandBufferImpl::CreateParams& a_params) const
	{
		return CommandBuffer(std::make_unique<CommandBufferImpl>(*this, a_params));
	}

	Rendering::CommandPool
	DeviceImpl::CreateCommandPool(const CommandPoolImpl::CreateParams& a_params) const
	{
		return CommandPool(std::make_unique<CommandPoolImpl>(*this, a_params));
	}

	Rendering::Image
	DeviceImpl::CreateImage(const ImageImpl::CreateParams& a_params) const
	{
		return Image(std::make_unique<ImageImpl>(*this, a_params));
	}

	Rendering::Shader
	DeviceImpl::CreateShader(const ShaderImpl::CreateParams& a_params) const
	{
		return Shader(std::make_unique<ShaderImpl>(*this, a_params));
	}

	Rendering::VertexBuffer
	DeviceImpl::CreateVertexBuffer(const Rendering::VertexBufferImpl::CreateParams& a_params) const
	{
		return VertexBuffer(std::make_unique<VertexBufferImpl>(*this, a_params));
	}

	Rendering::Semaphore
	DeviceImpl::CreateSemaphore() const
	{
		return Semaphore(std::make_unique<SemaphoreImpl>(*this));
	}

	Rendering::Fence
	DeviceImpl::CreateFence() const
	{
		return Fence(std::make_unique<FenceImpl>(*this));
	}

	void
	DeviceImpl::CreateInstance()
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
		auto layerProperties = m_context.enumerateInstanceLayerProperties();
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
		auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
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

		m_instance = vk::raii::Instance(m_context, createInfo);
	}

	void
	DeviceImpl::CreateDebugMessenger()
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
		m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	void
	DeviceImpl::CreateSurface()
	{
		OYL_PROFILE_FUNCTION();

		VkSurfaceKHR cSurface;
		auto glfwWindow = static_cast<GLFWwindow*>(m_window->GetNativeWindowHandle());
		if (glfwCreateWindowSurface(*m_instance, glfwWindow, nullptr, &cSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		m_surface = vk::raii::SurfaceKHR(m_instance, cSurface);
	}

	bool
	IsDeviceSuitable(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		vk::QueueFlags a_queueFlags,
		const std::vector<std::string>& a_requiredDeviceExtensions
	)
	{
		// Check if the physicalDevice supports the Vulkan 1.3 API version
		bool supportsVulkan1_3 = a_physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

		// Check if any of the queue families support requested operations
		auto queueFamilies = a_physicalDevice.getQueueFamilyProperties();
		bool supportsGraphics = std::ranges::any_of(
			queueFamilies,
			[&](const vk::QueueFamilyProperties& a_qfp)
			{
				auto mask = a_qfp.queueFlags & a_queueFlags;
				return mask == a_queueFlags;
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
	DeviceImpl::PickPhysicalDevice(CommandQueueFlags a_queueFlags)
	{
		OYL_PROFILE_FUNCTION();

		auto physicalDevices = m_instance.enumeratePhysicalDevices();
		const auto iter = std::ranges::find_if(
			physicalDevices,
			[&](const vk::raii::PhysicalDevice& a_physicalDevice)
			{
				auto vkQueueFlags = ToVkEnum(a_queueFlags);
				return IsDeviceSuitable(a_physicalDevice, vkQueueFlags, m_requiredDeviceExtensions);
			}
		);
		if (iter == physicalDevices.end())
		{
			throw std::runtime_error("failed to find a suitable GPU with Vulkan support!");
		}
		m_physicalDevice = *iter;
	}

	void
	DeviceImpl::CreateLogicalDevice()
	{
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();

		for (auto& [requestedFlag, index] : m_queueFamilyIndices)
		{
			// get the first index into queueFamilyProperties which supports the requested operations
			index = ~0u;
			for (uint32 propertiesIndex = 0; propertiesIndex < queueFamilyProperties.size(); propertiesIndex++)
			{
				auto queueFlags = queueFamilyProperties[propertiesIndex].queueFlags;
				vk::QueueFlags vkRequestedFlag = ToVkEnum(requestedFlag);

				if (queueFlags & vk::QueueFlagBits::eGraphics)
				{
					// If the queue supports graphics, we also want it to support SurfaceKHR
					if (!m_physicalDevice.getSurfaceSupportKHR(propertiesIndex, *m_surface))
						continue;

					// We want a unique queue for transfers
					if (requestedFlag == CommandQueueFlagBits::Transfer)
						continue;
				}

				if (queueFlags & vkRequestedFlag)
				{
					index = propertiesIndex;
					break;
				}
			}
			if (index == ~0u)
			{
				throw std::runtime_error("Could not find a queue for requested operations -> terminating");
			}
		}

		// Create a chain of feature structures
		vk::StructureChain featureChain = {
			vk::PhysicalDeviceFeatures2(),
			vk::PhysicalDeviceVulkan11Features { .shaderDrawParameters = true },
			vk::PhysicalDeviceVulkan13Features { .synchronization2 = true, .dynamicRendering = true },
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT { .extendedDynamicState = true }
		};

		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfo;
		float queuePriority = 0.5f;
		for (const auto& [flag, index] : m_queueFamilyIndices)
			deviceQueueCreateInfo.emplace_back(
				vk::DeviceQueueCreateInfo {
					.queueFamilyIndex = index,
					.queueCount = 1,
					.pQueuePriorities = &queuePriority,
				}
			);

		// Vulkan needs the extensions array as c-strings
		std::vector<const char*> requiredExtensions(m_requiredDeviceExtensions.size(), nullptr);
		for (size_t i = 0; i < m_requiredDeviceExtensions.size(); i++)
		{
			requiredExtensions[i] = m_requiredDeviceExtensions[i].c_str();
		}

		vk::DeviceCreateInfo deviceCreateInfo {
			.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
			.queueCreateInfoCount = (uint32) deviceQueueCreateInfo.size(),
			.pQueueCreateInfos = deviceQueueCreateInfo.data(),
			.enabledExtensionCount = (uint32) m_requiredDeviceExtensions.size(),
			.ppEnabledExtensionNames = requiredExtensions.data(),
		};

		m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
	}

	void
	DeviceImpl::CreateCommandQueues()
	{
		for (const auto& [flag, index] : m_queueFamilyIndices)
		{
			m_queues.emplace(
				flag,
				CommandQueueImpl(
					*this,
					{
						.queueFamilyIndex = index
					}
				)
			);
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
}
