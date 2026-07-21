#include "VulkanDevice.h"

#include <vulkan/vulkan_raii.hpp>

namespace Oyl::Rendering::Vulkan
{
	struct Device::Impl
	{
		std::vector<std::string> requiredDeviceExtensions;

		vk::raii::PhysicalDevice physicalDevice = nullptr;
		vk::raii::Device device = nullptr;

		vk::raii::Queue graphicsQueue = nullptr;
		uint32 graphicsQueueIndex = 0;

		void
		PickPhysicalDevice(const vk::raii::Instance& a_instance);
		void
		CreateLogicalDevice(const vk::raii::SurfaceKHR& a_surface);
	};

	Device::Device()
		: m_impl(nullptr) {}

	Device::Device(const DeviceParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		if (a_params.ppRequiredDeviceExtensionsData && a_params.requiredDeviceExtensionsLength > 0)
		{
			m_impl->requiredDeviceExtensions.reserve(a_params.requiredDeviceExtensionsLength);
			m_impl->requiredDeviceExtensions.insert(
				m_impl->requiredDeviceExtensions.end(),
				a_params.ppRequiredDeviceExtensionsData,
				a_params.ppRequiredDeviceExtensionsData + a_params.requiredDeviceExtensionsLength
			);
		}

		m_impl->PickPhysicalDevice(a_params.instance);
		m_impl->CreateLogicalDevice(a_params.surface);
	}

	Device::Device(Device&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	Device&
	Device::operator=(Device&& a_other) noexcept
	{
		if (this == &a_other)
			return *this;

		std::swap(m_impl, a_other.m_impl);
		return *this;
	}

	Device::~Device()
	{
		Device::Destroy();
	}

	bool
	Device::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return Rendering::Device::Destroy();

		m_impl->graphicsQueueIndex = 0;
		m_impl->graphicsQueue = nullptr;
		m_impl->device = nullptr;
		m_impl->physicalDevice = nullptr;
		m_impl->requiredDeviceExtensions.clear();

		m_impl.release();

		return Rendering::Device::Destroy();
	}

	bool
	Device::IsValid() const
	{
		return m_impl && m_impl->device != nullptr;
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
	Device::Impl::PickPhysicalDevice(const vk::raii::Instance& a_instance)
	{
		OYL_PROFILE_FUNCTION();

		auto physicalDevices = a_instance.enumeratePhysicalDevices();
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
	Device::Impl::CreateLogicalDevice(const vk::raii::SurfaceKHR& a_surface)
	{
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		// get the first index into queueFamilyProperties which supports both graphics and present
		graphicsQueueIndex = ~0u;
		for (uint32 qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
		{
			if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			    physicalDevice.getSurfaceSupportKHR(qfpIndex, *a_surface))
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
