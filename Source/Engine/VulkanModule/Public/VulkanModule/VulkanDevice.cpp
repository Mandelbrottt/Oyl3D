#include "VulkanDevice.h"

#include <Core/Containers/String.h>

#include "VulkanRenderingEnums.h"

namespace Oyl
{
	VulkanDevice::VulkanDevice(const vk::raii::Instance& a_vkInstance, const VulkanDeviceCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (a_params.requiredDeviceExtensions.Length() > 0)
		{
			m_requiredDeviceExtensions.reserve(a_params.requiredDeviceExtensions.Length());
			for (const char* extension : a_params.requiredDeviceExtensions)
				m_requiredDeviceExtensions.Add(extension);
		}

		for (uint32 i = 0; i < sizeof(a_params.commandQueueFlags) * 8; i++)
		{
			auto flag = CommandQueueFlagBits(1 << i);
			if (a_params.commandQueueFlags & flag)
			{
				m_queueFamilyIndices[flag] = 0;
			}
		}

		PickPhysicalDevice(a_vkInstance, a_params.commandQueueFlags);
		CreateLogicalDevice(a_params.presentTarget);
		CreateCommandQueues();
	}

	VulkanDevice::~VulkanDevice()
	{
		VulkanDevice::Destroy();
	}

	void
	VulkanDevice::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_queues.Clear();
		m_queueFamilyIndices.Clear();
		m_vkDevice = nullptr;
		m_vkPhysicalDevice = nullptr;
		m_requiredDeviceExtensions.Clear();
	}

	void
	VulkanDevice::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_vkDevice.waitIdle();
	}

	CommandBufferHandle
	VulkanDevice::CreateCommandBuffer(const CommandBufferCreateParams& a_params) const
	{
		return MakeSharedPtr<VulkanCommandBuffer>(*this, a_params);
	}

	CommandPoolHandle
	VulkanDevice::CreateCommandPool(const CommandPoolCreateParams& a_params) const
	{
		return MakeSharedPtr<VulkanCommandPool>(*this, a_params);
	}

	ImageHandle
	VulkanDevice::CreateImage(const ImageCreateParams& a_params) const
	{
		return MakeSharedPtr<VulkanImage>(*this, a_params);
	}

	ShaderHandle
	VulkanDevice::CreateShader(const ShaderCreateParams& a_params) const
	{
		return MakeSharedPtr<VulkanShader>(*this, a_params);
	}

	VertexBufferHandle
	VulkanDevice::CreateVertexBuffer(const VertexBufferCreateParams& a_params) const
	{
		return MakeSharedPtr<VulkanVertexBuffer>(*this, a_params);
	}

	SemaphoreHandle
	VulkanDevice::CreateSemaphore() const
	{
		return MakeSharedPtr<VulkanSemaphore>(*this);
	}

	FenceHandle
	VulkanDevice::CreateFence() const
	{
		return MakeSharedPtr<VulkanFence>(*this);
	}

	const VulkanCommandQueue*
	VulkanDevice::GetCommandQueue(CommandQueueFlagBits a_flag) const
	{
		if (!m_queues.ContainsKey(a_flag))
			return nullptr;

		return &m_queues.At(a_flag);
	}

	bool
	IsDeviceSuitable(
		const vk::raii::PhysicalDevice& a_physicalDevice,
		vk::QueueFlags a_queueFlags,
		const Array<String>& a_requiredDeviceExtensions
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
	VulkanDevice::PickPhysicalDevice(const vk::raii::Instance& a_vkInstance, CommandQueueFlags a_queueFlags)
	{
		OYL_PROFILE_FUNCTION();

		auto physicalDevices = a_vkInstance.enumeratePhysicalDevices();
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
		m_vkPhysicalDevice = *iter;
	}

	void
	VulkanDevice::CreateLogicalDevice(const VulkanPresentTarget* a_presentTarget)
	{
		OYL_PROFILE_FUNCTION();

		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_vkPhysicalDevice.getQueueFamilyProperties();

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
					// If we are given a PresentTarget, check for Present-To-Surface support
					if (!a_presentTarget)
						continue;

					auto& vkSurface = *a_presentTarget->GetVkSurface();
					if (!m_vkPhysicalDevice.getSurfaceSupportKHR(propertiesIndex, vkSurface))
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

		m_vkDevice = vk::raii::Device(m_vkPhysicalDevice, deviceCreateInfo);
	}

	void
	VulkanDevice::CreateCommandQueues()
	{
		for (const auto& [flag, index] : m_queueFamilyIndices)
		{
			m_queues.Emplace(
				flag,
				VulkanCommandQueue(
					*this,
					{
						.queueFamilyIndex = index
					}
				)
			);
		}
	}
}
